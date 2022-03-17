
cmake_minimum_required(VERSION 3.15)
project(modbus)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 20)
include(CheckCXXCompilerFlag)
include(conan.cmake)

# define project variables
set(TARGET_NAME ${PROJECT_NAME})

# include source directories
include_directories(
    "${CMAKE_SOURCE_DIR}/"
)

if(${UNIX})
    message("-- Setting up for Unix Environment")

    check_cxx_compiler_flag(-std=c++20 HAVE_FLAG_STD_CXX20)
    if(HAVE_FLAG_STD_CXX20)

        set( THREAD_CXX_FLAGS "-pthread")
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
    
        if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")
        elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            # set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines-ts")
            message(FATAL_ERROR "Clang coroutines is not supported in asio. Please add support")
        endif()

        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${THREAD_CXX_FLAGS}" )

        if(NOT CMAKE_INSTALL_INCLUDEDIR)
            set( CMAKE_INSTALL_INCLUDEDIR "/usr/include" )
        endif()

        if(NOT CMAKE_INSTALL_LIBDIR)
            set( CMAKE_INSTALL_LIBDIR "/usr/lib" )
        endif()

    else()
        message(FATAL_ERROR "This library requires C++20")
    endif()

else(${UNIX})
    message( FATAL_ERROR "Non-unix build is not supported. Feel free to add one.")

endif(${UNIX})

# integrate conan build process
if(NOT EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_cmake_run(CONANFILE conanfile.py
                    BASIC_SETUP CMAKE_TARGETS
                    BUILD missing)

    conan_cmake_autodetect(settings)

endif()

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
else()
    message(FATAL_ERROR "The file conanbuildinfo.cmake could not be generated")
endif()

set(INCLUDE_FILES
    "modbus/core/types.hpp"
    "modbus/core/is_message.hpp"
    "modbus/core/tcp_data_unit.hpp"
    "modbus/core/requests.hpp"
    "modbus/core/read_coils_request.hpp"
    "modbus/core/read_discrete_inputs_request.hpp"
    "modbus/core/read_holding_registers_request.hpp"
    "modbus/core/read_input_registers_request.hpp"
    "modbus/core/write_single_coil_request.hpp"
    "modbus/core/write_single_register_request.hpp"
    "modbus/core/write_multiple_coils_request.hpp"
    "modbus/core/write_multiple_registers_request.hpp"
    "modbus/core/mask_write_register_request.hpp"
    "modbus/core/read_write_registers_request.hpp"
    "modbus/core/responses.hpp"
    "modbus/core/read_coils_response.hpp"
    "modbus/core/read_discrete_inputs_response.hpp"
    "modbus/core/read_holding_registers_response.hpp"
    "modbus/core/read_input_registers_response.hpp"
    "modbus/core/write_single_coil_response.hpp"
    "modbus/core/write_single_register_response.hpp"
    "modbus/core/write_multiple_registers_response.hpp"
    "modbus/core/write_single_coil_request.hpp"
    "modbus/core/mask_write_register_response.hpp"
    "modbus/core/read_write_registers_response.hpp"
    "modbus/core/exception_response.hpp"
    "modbus/core/error.hpp"
    "modbus/core/modbus_response.hpp"
    "modbus/client/tcp_client.hpp"
    "modbus/client/client_config.hpp"
    "modbus/server/tcp_server.hpp"
    "modbus/server/tcp_session_manager.hpp"
    "modbus/server/tcp_session.hpp"
    "modbus/server/server_helper.hpp"
)

set(SOURCE_FILES
    "modbus/client.hpp"
    "modbus/server.hpp"
    "modbus/core/tcp_data_unit.cpp"
    "modbus/core/messages/read_coils_request.cpp"
    "modbus/core/messages/read_discrete_inputs_request.cpp"
    "modbus/core/messages/read_holding_registers_request.cpp"
    "modbus/core/messages/read_input_registers_request.cpp"
    "modbus/core/messages/write_single_coil_request.cpp"
    "modbus/core/messages/write_single_register_request.cpp"
    "modbus/core/messages/write_multiple_coils_request.cpp"
    "modbus/core/messages/write_multiple_registers_request.cpp"
    "modbus/core/messages/mask_write_register_request.cpp"
    "modbus/core/messages/read_write_registers_request.cpp"
    "modbus/core/messages/read_coils_response.cpp"
    "modbus/core/messages/read_discrete_inputs_response.cpp"
    "modbus/core/messages/read_holding_registers_response.cpp"
    "modbus/core/messages/read_input_registers_response.cpp"
    "modbus/core/messages/write_single_coil_response.cpp"
    "modbus/core/messages/write_single_register_response.cpp"
    "modbus/core/messages/write_multiple_coils_response.cpp"
    "modbus/core/messages/write_multiple_registers_response.cpp"
    "modbus/core/messages/mask_write_register_response.cpp"
    "modbus/core/messages/read_write_registers_response.cpp"
    "modbus/core/messages/exception_response.cpp"
    "modbus/core/error.cpp"
    "modbus/core/modbus_response.cpp"
    "modbus/client/tcp_client.cpp"
    "modbus/server/tcp_server.cpp"
    "modbus/server/tcp_session_manager.cpp"
    "modbus/server/tcp_session.cpp"
    )

add_library(${TARGET_NAME} STATIC ${SOURCE_FILES})

target_link_libraries(${TARGET_NAME} ${CONAN_LIBS})

# install headers
set(include_install_dir "${CMAKE_INSTALL_INCLUDEDIR}/modbus")
set(HEADER_BASE "${CMAKE_CURRENT_SOURCE_DIR}/modbus/")
install(
    DIRECTORY "${HEADER_BASE}"
    DESTINATION "${include_install_dir}"
)
# install libs
set(lib_install_dir "${CMAKE_INSTALL_LIBDIR}/modbus")
set(LIB_BASE "${CMAKE_BINARY_DIR}/lib/")
install(
    DIRECTORY "${LIB_BASE}"
    DESTINATION "${lib_install_dir}"
)

# create and configure the unit test target
option(BUILD_TESTING "Build the unit tests" ON)
option(RUN_E2E_TESTS "Run e2e tests" OFF)
message("-- BUILD_TESTING is ${BUILD_TESTING}")
message("-- RUN_E2E_TESTS is ${RUN_E2E_TESTS}")
if(BUILD_TESTING)
	enable_testing()
    add_subdirectory(test)
endif(BUILD_TESTING)