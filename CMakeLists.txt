
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
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/include/core"
    "${CMAKE_SOURCE_DIR}/include/core/messages"
    "${CMAKE_SOURCE_DIR}/include/client"
    "${CMAKE_SOURCE_DIR}/include/server"
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
    "include/core/types.hpp"
    "include/core/is_message.hpp"
    "include/core/tcp_data_unit.hpp"
    "include/core/requests.hpp"
    "include/core/read_coils_request.hpp"
    "include/core/read_discrete_inputs_request.hpp"
    "include/core/read_holding_registers_request.hpp"
    "include/core/read_input_registers_request.hpp"
    "include/core/write_single_coil_request.hpp"
    "include/core/write_single_register_request.hpp"
    "include/core/write_multiple_coils_request.hpp"
    "include/core/write_multiple_registers_request.hpp"
    "include/core/mask_write_register_request.hpp"
    "include/core/read_write_registers_request.hpp"
    "include/core/responses.hpp"
    "include/core/read_coils_response.hpp"
    "include/core/read_discrete_inputs_response.hpp"
    "include/core/read_holding_registers_response.hpp"
    "include/core/read_input_registers_response.hpp"
    "include/core/write_single_coil_response.hpp"
    "include/core/write_single_register_response.hpp"
    "include/core/write_multiple_registers_response.hpp"
    "include/core/write_single_coil_request.hpp"
    "include/core/mask_write_register_response.hpp"
    "include/core/read_write_registers_response.hpp"
    "include/core/exception_response.hpp"
    "include/core/error.hpp"
    "include/core/modbus_response.hpp"
    "include/client/tcp_client.hpp"
    "include/client/client_config.hpp"
    "include/server/tcp_server.hpp"
    "include/server/tcp_session_manager.hpp"
    "include/server/tcp_session.hpp"
    "include/server/server_helper.hpp"
)

set(SOURCE_FILES
    "src/core/tcp_data_unit.cpp"
    "src/core/messages/read_coils_request.cpp"
    "src/core/messages/read_discrete_inputs_request.cpp"
    "src/core/messages/read_holding_registers_request.cpp"
    "src/core/messages/read_input_registers_request.cpp"
    "src/core/messages/write_single_coil_request.cpp"
    "src/core/messages/write_single_register_request.cpp"
    "src/core/messages/write_multiple_coils_request.cpp"
    "src/core/messages/write_multiple_registers_request.cpp"
    "src/core/messages/mask_write_register_request.cpp"
    "src/core/messages/read_write_registers_request.cpp"
    "src/core/messages/read_coils_response.cpp"
    "src/core/messages/read_discrete_inputs_response.cpp"
    "src/core/messages/read_holding_registers_response.cpp"
    "src/core/messages/read_input_registers_response.cpp"
    "src/core/messages/write_single_coil_response.cpp"
    "src/core/messages/write_single_register_response.cpp"
    "src/core/messages/write_multiple_coils_response.cpp"
    "src/core/messages/write_multiple_registers_response.cpp"
    "src/core/messages/mask_write_register_response.cpp"
    "src/core/messages/read_write_registers_response.cpp"
    "src/core/messages/exception_response.cpp"
    "src/core/error.cpp"
    "src/core/modbus_response.cpp"
    "src/client/tcp_client.cpp"
    "src/server/tcp_server.cpp"
    "src/server/tcp_session_manager.cpp"
    "src/server/tcp_session.cpp"
    )

add_library(${TARGET_NAME} STATIC ${SOURCE_FILES})

target_link_libraries(${TARGET_NAME} ${CONAN_LIBS})

if(${STATEMACHINE_LOGGING})
    message("-- Adding state machine logging")
    target_compile_definitions(${TARGET_NAME} PRIVATE "STATEMACHINE_LOGGING=1")
endif(${STATEMACHINE_LOGGING})

# install headers
set(include_install_dir "${CMAKE_INSTALL_INCLUDEDIR}/modbus")
set(HEADER_BASE "${CMAKE_CURRENT_SOURCE_DIR}/include/")
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
option(BuildTests "Build the unit tests" ON)
message("-- BuildTests is ${BuildTests}")
if(BuildTests)
	enable_testing()
    add_subdirectory(test)
endif(BuildTests)