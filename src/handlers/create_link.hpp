#pragma once

#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>

/// @defgroup handlers HTTP Handlers
/// @brief HTTP обработчики запросов ShortLinker сервиса

namespace ShortLinker {

/// @ingroup handlers
/// @brief Обработчик создания короткой ссылки
///
/// Принимает POST-запрос на `/shorten` с JSON-телом вида:
/// @code{.json}
/// { "url": "https://example.com/long/path" }
/// @endcode
///
/// При успехе возвращает HTTP 201 с JSON:
/// @code{.json}
/// { "id": "aBc12DeF", "short_url": "/aBc12DeF" }
/// @endcode
///
/// @par Возможные ответы:
/// - `201 Created`    — ссылка успешно создана
/// - `400 Bad Request` — невалидный JSON, нет поля `url` или оно пустое
/// - `500 Internal Server Error` — не удалось сгенерировать уникальный ID
class CreateLink final : public userver::server::handlers::HttpHandlerBase {
 public:
  /// @brief Имя компонента для регистрации в userver
  static constexpr std::string_view kName = "handler-create-link";

  /// @brief Конструктор компонента
  /// @param config конфигурация компонента из static_config.yaml
  /// @param context контекст компонентов userver для поиска зависимостей
  CreateLink(const userver::components::ComponentConfig& config,
             const userver::components::ComponentContext& context);

  /// @brief Обрабатывает HTTP-запрос создания короткой ссылки
  /// @param request входящий HTTP-запрос
  /// @return JSON-строка с результатом операции
  /// @throws userver::server::handlers::ClientError при ошибке клиента
  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override;

 private:
  /// @brief Пул соединений с PostgreSQL
  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace ShortLinker
