include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

# include source directories
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/core"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/client"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/server"
)


set(MODBUS_RESPONSE "modbus_response_test")
add_executable(${MODBUS_RESPONSE}
        "response_data_test.cpp"
)
target_include_directories(${MODBUS_RESPONSE} PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(${MODBUS_RESPONSE} ${TARGET_NAME} ${CONAN_LIBS})
add_test(NAME ${MODBUS_RESPONSE} COMMAND $<TARGET_FILE:${MODBUS_RESPONSE}>)


set(TCP_DATA_UNIT "tcp_data_unit_test")
add_executable(${TCP_DATA_UNIT}
        "tcp_data_unit_test.cpp"
)
target_include_directories(${TCP_DATA_UNIT} PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(${TCP_DATA_UNIT} ${TARGET_NAME} ${CONAN_LIBS})
add_test(NAME ${TCP_DATA_UNIT} COMMAND $<TARGET_FILE:${TCP_DATA_UNIT}>)


set(RESPONSE "response-test")
add_executable(${RESPONSE}
        "response_test.cpp"
)
target_include_directories(${RESPONSE} PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(${RESPONSE} ${TARGET_NAME} ${CONAN_LIBS})
add_test(NAME ${RESPONSE} COMMAND $<TARGET_FILE:${RESPONSE}>)


set(REQUEST "request-test")
add_executable(${REQUEST}
        "request_test.cpp"
)
target_include_directories(${REQUEST} PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(${REQUEST} ${TARGET_NAME} ${CONAN_LIBS})
add_test(NAME ${REQUEST} COMMAND $<TARGET_FILE:${REQUEST}>)


set(REG_OPS "reg-ops-test")
add_executable(${REG_OPS}
        "register_operations_test.cpp"
)
target_include_directories(${REG_OPS} PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(${REG_OPS} ${TARGET_NAME} ${CONAN_LIBS})
add_test(NAME ${REG_OPS} COMMAND $<TARGET_FILE:${REG_OPS}>)


set(LEGAL_ADDRESS "legal-address-test")
add_executable(${LEGAL_ADDRESS}
        "legal_address_test.cpp"
)
target_include_directories(${LEGAL_ADDRESS} PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(${LEGAL_ADDRESS} ${TARGET_NAME} ${CONAN_LIBS})
add_test(NAME ${LEGAL_ADDRESS} COMMAND $<TARGET_FILE:${LEGAL_ADDRESS}>)


set(BIT_OPS "bit-operations-test")
add_executable(${BIT_OPS}
        "bit_operations_test.cpp"
)
target_include_directories(${BIT_OPS} PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(${BIT_OPS} ${TARGET_NAME} ${CONAN_LIBS})
add_test(NAME ${BIT_OPS} COMMAND $<TARGET_FILE:${BIT_OPS}>)


set(E2E_TCP_CLIENT_TEST "e2e-tcp-client-test")
add_executable(${E2E_TCP_CLIENT_TEST}
        "e2e_tcp_client_test.cpp"
)
target_include_directories(${E2E_TCP_CLIENT_TEST} PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(${E2E_TCP_CLIENT_TEST} ${TARGET_NAME} ${CONAN_LIBS})
add_test(NAME ${E2E_TCP_CLIENT_TEST} COMMAND $<TARGET_FILE:${E2E_TCP_CLIENT_TEST}>)


set(TCP_SERVER_TEST "tcp-server-test")
add_executable(${TCP_SERVER_TEST}
        "tcp_server_test.cpp"
)
target_include_directories(${TCP_SERVER_TEST} PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(${TCP_SERVER_TEST} ${TARGET_NAME} ${CONAN_LIBS})
add_test(NAME ${TCP_SERVER_TEST} COMMAND $<TARGET_FILE:${TCP_SERVER_TEST}>)
