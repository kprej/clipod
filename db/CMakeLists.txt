project(
    clipod-db
    VERSION 0.1
    LANGUAGES CXX
)

add_executable(
    ${PROJECT_NAME}
    main.cpp
)

target_link_libraries(
    ${PROJECT_NAME} 
    PUBLIC
    cxxopts
    clipod
)
