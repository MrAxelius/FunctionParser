cmake_minimum_required(VERSION 3.20)
project(FunctionParser VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
    add_compile_options(/utf-8 /W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

# Opciones de build
option(BUILD_SHARED_LIBS "Build shared library" OFF)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" OFF)

# Biblioteca principal
add_library(FunctionParser
    src/Lexer/Lexer.cpp
    src/Parser/Parser.cpp
    src/Expression.cpp
)

# Directorios de inclusión públicos
target_include_directories(FunctionParser
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_library(FunctionParser::FunctionParser ALIAS FunctionParser)

if(BUILD_EXAMPLES)
    add_executable(parser_cli examples/example_cli.cpp)
    target_link_libraries(parser_cli PRIVATE FunctionParser)
endif()

# Para el catch2
if(BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.7.1
    )
    FetchContent_MakeAvailable(Catch2)

    add_executable(parser_tests
        tests/test_lexer.cpp
        tests/test_parser.cpp
        tests/test_evaluador.cpp
        tests/test_muestreo.cpp
    )
    target_link_libraries(parser_tests PRIVATE FunctionParser Catch2::Catch2WithMain)
    target_include_directories(parser_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

    include(CTest)
    include(Catch)
    catch_discover_tests(parser_tests)
endif()

install(DIRECTORY include/ DESTINATION include)