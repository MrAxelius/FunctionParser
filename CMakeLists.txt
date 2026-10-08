cmake_minimum_required(VERSION 3.20)
project(FunctionParser VERSION 1.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Opciones de build
option(BUILD_SHARED_LIBS "Build shared library" OFF)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" OFF)
option(FUNCTIONPARSER_WERROR "Treat compiler warnings as errors" OFF)
option(FUNCTIONPARSER_BUILD_BENCHMARKS "Build the benchmark executable" OFF)

# ---------------------------------------------------------------------------
# Avisos del compilador, como objetivo INTERFACE.
#
# Esto sustituye al add_compile_options() que habia aqui, y el motivo importa:
# add_compile_options() aplica a TODO lo que se cree en este directorio y por
# debajo, incluido lo que trae FetchContent. Con -Werror eso significaria
# compilar Catch2 con nuestras reglas y fallar el build por avisos de codigo
# que no es nuestro. Un objetivo INTERFACE solo llega a quien lo enlaza.
# ---------------------------------------------------------------------------
add_library(fp_avisos INTERFACE)

if(MSVC)
    target_compile_options(fp_avisos INTERFACE /utf-8 /W4)

    if(FUNCTIONPARSER_WERROR)
        target_compile_options(fp_avisos INTERFACE /WX)
    endif()
else()
    target_compile_options(fp_avisos INTERFACE -Wall -Wextra -Wconversion)

    if(FUNCTIONPARSER_WERROR)
        target_compile_options(fp_avisos INTERFACE -Werror)
    endif()
endif()

# Biblioteca principal
add_library(FunctionParser
    src/Lexer/Lexer.cpp
    src/Parser/Parser.cpp
    src/Expression.cpp
)

target_link_libraries(FunctionParser PRIVATE fp_avisos)

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
    target_link_libraries(parser_cli PRIVATE FunctionParser::FunctionParser fp_avisos)
endif()

# ---------------------------------------------------------------------------
# Benchmark. Enlaza por el alias y sin src/ en la ruta de inclusión, igual que
# api_tests: es un consumidor externo, y si el header público dejara de
# compilar por si solo, este objetivo lo detecta.
#
# Apagado por defecto. En Release hereda /O2 y NDEBUG, asi que los assert del
# evaluador quedan fuera de la medicion sin depender de que nadie lo recuerde.
# ---------------------------------------------------------------------------
if(FUNCTIONPARSER_BUILD_BENCHMARKS)
    add_executable(fp_benchmark benchmark/benchmark.cpp)
    target_link_libraries(fp_benchmark PRIVATE FunctionParser::FunctionParser fp_avisos)
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

    # Tests internos: ven src/, prueban las capas por dentro
    add_executable(parser_tests
        tests/test_lexer.cpp
        tests/test_parser.cpp
        tests/test_evaluador.cpp
        tests/test_muestreo.cpp
    )
    target_link_libraries(parser_tests PRIVATE FunctionParser Catch2::Catch2WithMain fp_avisos)
    target_include_directories(parser_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

    # Tests de la API pública: SIN src/ en la ruta de inclusión, a propósito.
    # Solo pueden ver lo que ve un consumidor de la biblioteca.
    add_executable(api_tests
        tests/test_api_publica.cpp
    )
    target_link_libraries(api_tests PRIVATE FunctionParser::FunctionParser Catch2::Catch2WithMain fp_avisos)

    enable_testing()
    include(Catch)
    catch_discover_tests(parser_tests)
    catch_discover_tests(api_tests)
endif()

option(FUNCTIONPARSER_BUILD_BENCHMARKS "Build the benchmark executable" OFF)

install(DIRECTORY include/ DESTINATION include)