#include <redirect.hpp>

#include <userver/components/component.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/component.hpp>

namespace ShortLinker {

Redirect::Redirect(const userver::components::ComponentConfig& config,
                   const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      pg_cluster_(
          context.FindComponent<userver::components::Postgres>("postgres-db")
              .GetCluster()) {}

std::string Redirect::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
  auto& response = request.GetHttpResponse();

  const auto& id = request.GetPathArg("id");
  if (id.empty()) {
    response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
    return "Missing link id";
  }

  auto result = pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kSlave,
      "SELECT target_url FROM short_links WHERE id = $1", id);

  if (result.IsEmpty()) {
    response.SetStatus(userver::server::http::HttpStatus::kNotFound);
    return "Link not found";
  }

  const auto target_url = result.AsSingleRow<std::string>();

  response.SetStatus(userver::server::http::HttpStatus::kFound);
  response.SetHeader(std::string{"Location"}, std::string{target_url});
  return {};
}

}  // namespace ShortLinker
