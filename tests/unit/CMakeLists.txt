add_executable(${PROJECT_NAME}_unittest short_links_test.cpp)
target_link_libraries(${PROJECT_NAME}_unittest PRIVATE userver::utest)
add_google_tests(${PROJECT_NAME}_unittest)
