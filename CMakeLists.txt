cmake_minimum_required(VERSION 3.16)

project(estrellio_logging_lib LANGUAGES C CXX)

option(ESTRELLIO_LOGGING_BUILD_C "Build the C logging library" ON)
option(ESTRELLIO_LOGGING_BUILD_CPP "Build the C++ logging library" ON)
option(ESTRELLIO_LOGGING_BUILD_TESTS "Build native logging smoke tests" ${PROJECT_IS_TOP_LEVEL})

if(ESTRELLIO_LOGGING_BUILD_C)
    add_library(estrellio_logging_c STATIC
        src/c/estrellio_logging_lib.c
    )
    add_library(estrellio_logging::c ALIAS estrellio_logging_c)
    target_include_directories(estrellio_logging_c
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/src/c
    )
    target_compile_features(estrellio_logging_c PUBLIC c_std_11)
endif()

if(ESTRELLIO_LOGGING_BUILD_CPP)
    if(NOT ESTRELLIO_LOGGING_BUILD_C)
        message(FATAL_ERROR "ESTRELLIO_LOGGING_BUILD_CPP requires ESTRELLIO_LOGGING_BUILD_C")
    endif()
    add_library(estrellio_logging_cpp STATIC
        src/cpp/estrellio_logging_lib.cpp
    )
    add_library(estrellio_logging::cpp ALIAS estrellio_logging_cpp)
    target_include_directories(estrellio_logging_cpp
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/src/cpp
    )
    target_link_libraries(estrellio_logging_cpp PUBLIC estrellio_logging::c)
    target_compile_features(estrellio_logging_cpp PUBLIC cxx_std_11)
endif()

if(ESTRELLIO_LOGGING_BUILD_TESTS)
    enable_testing()

    if(ESTRELLIO_LOGGING_BUILD_C)
        add_executable(estrellio_logging_c_smoke tests/native/test_c_smoke.c)
        target_link_libraries(estrellio_logging_c_smoke PRIVATE estrellio_logging::c)
        add_test(
            NAME estrellio_logging_c_smoke
            COMMAND estrellio_logging_c_smoke ${CMAKE_CURRENT_BINARY_DIR}/estrellio_c_smoke.log
        )
    endif()

    if(ESTRELLIO_LOGGING_BUILD_CPP)
        add_executable(estrellio_logging_cpp_smoke tests/native/test_cpp_smoke.cpp)
        target_link_libraries(estrellio_logging_cpp_smoke PRIVATE estrellio_logging::cpp)
        add_test(
            NAME estrellio_logging_cpp_smoke
            COMMAND estrellio_logging_cpp_smoke ${CMAKE_CURRENT_BINARY_DIR}/estrellio_cpp_smoke.log
        )
    endif()
endif()
