#include <create_link.hpp>

#include <random>

#include <fmt/format.h>

#include <userver/components/component.hpp>
#include <userver/formats/json.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/component.hpp>

namespace service2_13 {

namespace {

constexpr std::size_t kIdLength = 8;
constexpr std::string_view kAlphabet =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

std::string GenerateId() {
  thread_local std::mt19937 gen{std::random_device{}()};
  std::uniform_int_distribution<std::size_t> dist(0, kAlphabet.size() - 1);
  std::string id;
  id.reserve(kIdLength);
  for (std::size_t i = 0; i < kIdLength; ++i) {
    id += kAlphabet[dist(gen)];
  }
  return id;
}

constexpr std::size_t kMaxRetries = 5;

}  // namespace

CreateLink::CreateLink(const userver::components::ComponentConfig& config,
                       const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      pg_cluster_(
          context.FindComponent<userver::components::Postgres>("postgres-db")
              .GetCluster()) {}

std::string CreateLink::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
  auto& response = request.GetHttpResponse();
  response.SetContentType("application/json");

  const auto& body = request.RequestBody();
  userver::formats::json::Value json;
  try {
    json = userver::formats::json::FromString(body);
  } catch (const std::exception&) {
    response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    return R"({"error":"invalid JSON"})";
  }

  if (!json.HasMember("url")) {
    response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    return R"({"error":"missing 'url' field"})";
  }

  const auto target_url = json["url"].As<std::string>();
  if (target_url.empty()) {
    response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    return R"({"error":"'url' must not be empty"})";
  }

  for (std::size_t attempt = 0; attempt < kMaxRetries; ++attempt) {
    const auto id = GenerateId();
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO short_links (id, target_url) "
        "VALUES ($1, $2) "
        "ON CONFLICT (id) DO NOTHING "
        "RETURNING id",
        id, target_url);

    if (!result.IsEmpty()) {
      userver::formats::json::ValueBuilder resp;
      resp["id"] = id;
      resp["short_url"] = fmt::format("/{}", id);
      response.SetStatus(userver::server::http::HttpStatus::kCreated);
      return userver::formats::json::ToString(resp.ExtractValue());
    }
  }

  response.SetStatus(userver::server::http::HttpStatus::kInternalServerError);
  return R"({"error":"failed to generate unique id"})";
}

}  // namespace service2_13
