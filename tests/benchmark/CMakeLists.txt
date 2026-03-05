add_executable(${PROJECT_NAME}_benchmark short_links_benchmark.cpp)
target_link_libraries(${PROJECT_NAME}_benchmark PRIVATE userver::ubench)
add_google_benchmark_tests(${PROJECT_NAME}_benchmark)
