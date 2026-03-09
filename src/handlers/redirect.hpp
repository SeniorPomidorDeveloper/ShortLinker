#pragma once

#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>

namespace ShortLinker {

/// @ingroup handlers
/// @brief Обработчик редиректа по короткой ссылке
///
/// Принимает GET-запрос на `/r/{id}` и перенаправляет на оригинальный URL.
///
/// @par Возможные ответы:
/// - `302 Found`      — редирект на оригинальный URL (заголовок `Location`)
/// - `400 Bad Request` — отсутствует параметр `id` в пути
/// - `404 Not Found`  — ссылка с данным `id` не найдена
class Redirect final : public userver::server::handlers::HttpHandlerBase {
 public:
  /// @brief Имя компонента для регистрации в userver
  static constexpr std::string_view kName = "handler-redirect";

  /// @brief Конструктор компонента
  /// @param config конфигурация компонента из static_config.yaml
  /// @param context контекст компонентов userver для поиска зависимостей
  Redirect(const userver::components::ComponentConfig& config,
           const userver::components::ComponentContext& context);

  /// @brief Обрабатывает HTTP-запрос редиректа
  ///
  /// Ищет короткую ссылку по `id` в базе данных (replica) и выполняет
  /// HTTP 302 редирект на оригинальный URL.
  ///
  /// @param request входящий HTTP-запрос, содержащий path-параметр `id`
  /// @return пустую строку при успешном редиректе
  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override;

 private:
  /// @brief Пул соединений с PostgreSQL
  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace ShortLinker
