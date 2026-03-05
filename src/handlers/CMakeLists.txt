add_library(
    ${PROJECT_NAME}_handlers OBJECT
    create_link.cpp
    redirect.cpp
)

target_include_directories(
    ${PROJECT_NAME}_handlers
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
    ${PROJECT_NAME}_handlers
    PUBLIC userver::postgresql
)
