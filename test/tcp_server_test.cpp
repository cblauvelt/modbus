#include "client/tcp_client.hpp"
#include "core/modbus_response.hpp"
#include "core/requests.hpp"
#include "core/responses.hpp"
#include "core/types.hpp"
#include "server/server_helper.hpp"
#include "server/tcp_server.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {

using namespace modbus;
using namespace std;

const std::string DEFAULT_HOST = "127.0.0.1";
const std::string ENV_HOST = "MODBUS_SERVER_HOST";
const std::string ENV_SLOW_HOST = "MODBUS_SLOW_SERVER_HOST";
const std::string DEFAULT_PORT = "503";
const std::string ENV_PORT = "MODBUS_SERVER_PORT";
const std::string DEFAULT_SLOW_PORT = "504";
const std::string ENV_SLOW_PORT = "MODBUS_SLOW_SERVER_PORT";
const std::chrono::milliseconds SERVER_WAIT = 500ms;

std::optional<std::string> get_env_var(std::string const& key) {
    char* val = getenv(key.c_str());
    return (val == NULL) ? std::nullopt : std::optional(std::string(val));
}

constexpr unsigned int num_bytes(unsigned int num_coils) {
    unsigned int nBytes = num_coils / 8;
    if (num_coils % 8) {
        nBytes++;
    }

    return nBytes;
}

awaitable<void> wait_for(std::chrono::milliseconds timeout) {
    // wait for the server to start
    cpool::timer timer(co_await asio::this_coro::executor);
    co_await timer.async_wait(timeout);
}

struct request_handler {
    mutex memorySpaceMutex;
    vector<buffer_t> memorySpace;
    vector<buffer_t> validAddresses;

    request_handler() {
        memorySpace = {
            {0x9A, 0xA9, 0x01}, // Coil statuses
            {0xA9, 0x9A, 0x06}, // Input statuses
            {0x0F, 0xF0, 0x0F, 0xF0, 0x00, 0x00, 0x0F,
             0xF0}, // Holding registers
            {0xF0, 0x0F, 0xF0, 0x0F, 0x00, 0x00, 0xF0, 0x0F} // Input registers
        };

        // Valid addresses are marked by a bit mask of 1
        // For the input status and coils, if addresses
        // 1-16, 18-19 are available then the bitmask is
        // 0xFF, 0xFF, 0x06
        // For the input and holding registers, if addresses
        // 1, 2, 3, 4, 7, and 8 are available but
        // 5 and 6 are not, the bitmask is 0xCF or 0b11001111
        validAddresses = {
            {0xFF, 0xFF, 0x07},
            {0xFF, 0xFF, 0x07},
            {0x0B},
            {0x0B},

        };
    }

    awaitable<tcp_data_unit> operator()(const tcp_data_unit& requestDataUnit) {
        auto function_code = requestDataUnit.function_code();
        auto transaction_id = requestDataUnit.transaction_id();
        auto unit_id = requestDataUnit.unit_id();
        vector<uint8_t> inputs;

        print_logging_handler(
            log_level::debug,
            fmt::format("Function Code: {}; Transaction ID: {}",
                        (uint)function_code, (uint)transaction_id));

        if (function_code == function_code_t::read_discrete_inputs) {
            auto optionalRequest =
                requestDataUnit.pdu<read_discrete_inputs_request>();
            int dataRow = 1;

            // If the function code and the request do not agree, co_return an
            // error
            if (!optionalRequest) {
                co_return tcp_data_unit(
                    transaction_id,
                    exception_response(
                        unit_id, function_code,
                        exception_code_t::server_device_failure));
            }

            auto request = optionalRequest.value();
            auto requestBitmap =
                create_request_bit_map(request.start_address, request.length);

            // Check if any requested addresses are illegal
            if (!legal_address(validAddresses[dataRow], requestBitmap,
                               request.start_address)) {
                co_return tcp_data_unit(
                    transaction_id,
                    exception_response(unit_id, function_code,
                                       exception_code_t::illegal_data_address));
            }

            lock_guard<mutex> lock(memorySpaceMutex);
            const vector<uint8_t>& data = memorySpace[dataRow];

            inputs = copy_data_bits(data, requestBitmap, request.start_address,
                                    request.length);

            co_return tcp_data_unit(
                transaction_id, read_discrete_inputs_response(unit_id, inputs));
        } else if (function_code == function_code_t::read_coils) {
            auto optionalRequest = requestDataUnit.pdu<read_coils_request>();
            int dataRow = 0;

            // If the function code and the request do not agree, co_return an
            // error
            if (!optionalRequest) {
                co_return tcp_data_unit(
                    transaction_id,
                    exception_response(
                        unit_id, function_code,
                        exception_code_t::server_device_failure));
            }
            auto request = optionalRequest.value();
            auto requestBitmap =
                create_request_bit_map(request.start_address, request.length);

            // Check if any requested addresses are illegal
            if (!legal_address(validAddresses[dataRow], requestBitmap,
                               request.start_address)) {
                co_return tcp_data_unit(
                    transaction_id,
                    exception_response(unit_id, function_code,
                                       exception_code_t::illegal_data_address));
            }

            lock_guard<mutex> lock(memorySpaceMutex);
            const vector<uint8_t>& data = memorySpace[dataRow];

            inputs = copy_data_bits(data, requestBitmap, request.start_address,
                                    request.length);

            co_return tcp_data_unit(transaction_id,
                                    read_coils_response(unit_id, inputs));
        } else if (function_code == function_code_t::write_single_coil) {
            auto optionalRequest =
                requestDataUnit.pdu<write_single_coil_request>();
            int dataRow = 0;

            // If the function code and the request do not agree, co_return an
            // error
            if (!optionalRequest) {
                co_return tcp_data_unit(
                    transaction_id,
                    exception_response(
                        unit_id, function_code,
                        exception_code_t::server_device_failure));
            }

            auto request = optionalRequest.value();
            auto requestBitmap =
                create_request_bit_map(request.start_address, 1);

            // Check if any requested addresses are illegal
            if (!legal_address(validAddresses[dataRow], requestBitmap,
                               request.start_address)) {
                co_return tcp_data_unit(
                    transaction_id,
                    exception_response(unit_id, function_code,
                                       exception_code_t::illegal_data_address));
            }

            // Write the coil
            lock_guard<mutex> lock(memorySpaceMutex);
            if (request.value == coil_status_t::on) {
                write_coil(memorySpace[dataRow], request.value,
                           request.start_address);
            }

            co_return tcp_data_unit(
                transaction_id,
                request // A proper response is to mirror the request
            );
        } else if (function_code == function_code_t::write_multiple_coils) {
            auto optionalRequest =
                requestDataUnit.pdu<write_multiple_coils_request>();
            int dataRow = 0;

            // If the function code and the request do not agree, co_return an
            // error
            if (!optionalRequest) {
                co_return tcp_data_unit(
                    transaction_id,
                    exception_response(
                        unit_id, function_code,
                        exception_code_t::server_device_failure));
            }

            auto request = optionalRequest.value();
            auto requestBitmap =
                create_request_bit_map(request.start_address, request.length);

            // Check if any requested addresses are illegal
            if (!legal_address(validAddresses[dataRow], requestBitmap,
                               request.start_address)) {
                co_return tcp_data_unit(
                    transaction_id,
                    exception_response(unit_id, function_code,
                                       exception_code_t::illegal_data_address));
            }

            lock_guard<mutex> lock(memorySpaceMutex);
            vector<uint8_t>& data = memorySpace[dataRow];

            write_coils(data, requestBitmap, request.values,
                        request.start_address);

            co_return tcp_data_unit(
                transaction_id,
                write_multiple_coils_response(unit_id, request.start_address,
                                              request.length));
        } else if (function_code == function_code_t::read_holding_registers) {
            auto optionalRequest =
                requestDataUnit.pdu<read_holding_registers_request>();
            int dataRow = 2;

            // If the function code and the request do not agree, co_return an
            // error
            if (!optionalRequest) {
                co_return tcp_data_unit(
                    transaction_id,
                    exception_response(
                        unit_id, function_code,
                        exception_code_t::server_device_failure));
            }

            auto request = optionalRequest.value();
            auto requestBitmap =
                create_request_bit_map(request.start_address, request.length);

            // Check if any requested addresses are illegal
            if (!legal_address(validAddresses[dataRow], requestBitmap,
                               request.start_address)) {
                co_return tcp_data_unit(
                    transaction_id,
                    exception_response(unit_id, function_code,
                                       exception_code_t::illegal_data_address));
            }

            lock_guard<mutex> lock(memorySpaceMutex);
            const vector<uint8_t>& data = memorySpace[dataRow];

            inputs = copy_data_registers(data, request.start_address,
                                         request.length);

            co_return tcp_data_unit(
                transaction_id,
                read_holding_registers_response(unit_id, inputs));
        } else if (function_code == function_code_t::read_input_registers) {
            auto optionalRequest =
                requestDataUnit.pdu<read_input_registers_request>();
            int dataRow = 3;

            // If the function code and the request do not agree, co_return an
            // error
            if (!optionalRequest) {
                co_return tcp_data_unit(
                    transaction_id,
                    exception_response(
                        unit_id, function_code,
                        exception_code_t::server_device_failure));
            }

            auto request = optionalRequest.value();
            auto requestBitmap =
                create_request_bit_map(request.start_address, request.length);

            // Check if any requested addresses are illegal
            if (!legal_address(validAddresses[dataRow], requestBitmap,
                               request.start_address)) {
                co_return tcp_data_unit(
                    transaction_id,
                    exception_response(unit_id, function_code,
                                       exception_code_t::illegal_data_address));
            }

            lock_guard<mutex> lock(memorySpaceMutex);
            const vector<uint8_t>& data = memorySpace[dataRow];

            inputs = copy_data_registers(data, request.start_address,
                                         request.length);

            co_return tcp_data_unit(
                transaction_id, read_input_registers_response(unit_id, inputs));
        } else if (function_code == function_code_t::write_single_register) {
            auto optionalRequest =
                requestDataUnit.pdu<write_single_register_request>();
            int dataRow = 2;

            // If the function code and the request do not agree, co_return an
            // error
            if (!optionalRequest) {
                co_return tcp_data_unit(
                    transaction_id,
                    exception_response(
                        unit_id, function_code,
                        exception_code_t::server_device_failure));
            }

            auto request = optionalRequest.value();
            auto requestBitmap =
                create_request_bit_map(request.start_address, 1);

            // Check if any requested addresses are illegal
            if (!legal_address(validAddresses[dataRow], requestBitmap,
                               request.start_address)) {
                co_return tcp_data_unit(
                    transaction_id,
                    exception_response(unit_id, function_code,
                                       exception_code_t::illegal_data_address));
            }

            lock_guard<mutex> lock(memorySpaceMutex);
            vector<uint8_t>& data = memorySpace[dataRow];

            write_register(request.value, data, request.start_address);

            co_return tcp_data_unit(transaction_id, request);
        } else if (function_code == function_code_t::write_multiple_registers) {
            auto optionalRequest =
                requestDataUnit.pdu<write_multiple_registers_request>();
            int dataRow = 2;

            // If the function code and the request do not agree, co_return an
            // error
            if (!optionalRequest) {
                co_return tcp_data_unit(
                    transaction_id,
                    exception_response(
                        unit_id, function_code,
                        exception_code_t::server_device_failure));
            }

            auto request = optionalRequest.value();
            auto requestBitmap =
                create_request_bit_map(request.start_address, request.length);

            // Check if any requested addresses are illegal
            if (!legal_address(validAddresses[dataRow], requestBitmap,
                               request.start_address)) {
                co_return tcp_data_unit(
                    transaction_id,
                    exception_response(unit_id, function_code,
                                       exception_code_t::illegal_data_address));
            }

            lock_guard<mutex> lock(memorySpaceMutex);
            vector<uint8_t>& data = memorySpace[dataRow];

            write_registers(request.values, data, request.start_address,
                            request.length);

            co_return tcp_data_unit(transaction_id, request);
        }

        co_return tcp_data_unit(
            transaction_id,
            exception_response(unit_id, function_code,
                               exception_code_t::illegal_function));
    }
};

awaitable<tcp_data_unit> false_handler(const tcp_data_unit& request) {
    auto function_code = request.function_code();
    auto transaction_id = request.transaction_id();
    auto unit_id = request.unit_id();

    co_return tcp_data_unit(
        transaction_id, exception_response(unit_id, function_code,
                                           exception_code_t::illegal_function));
}

awaitable<tcp_data_unit> slow_handler(const tcp_data_unit& requestDataUnit) {
    auto function_code = requestDataUnit.function_code();
    auto transaction_id = requestDataUnit.transaction_id();
    auto unit_id = requestDataUnit.unit_id();
    std::vector<uint8_t> input_registers{0xF0, 0x0F, 0xF0, 0x0F,
                                         0x00, 0x00, 0xF0, 0x0F};
    std::vector<uint8_t> holding_registers{0x0F, 0xF0, 0x0F, 0xF0,
                                           0x00, 0x00, 0x0F, 0xF0};
    std::vector<uint8_t> validAddresses{0x0B};
    std::vector<uint8_t> inputs{0x00};

    if (function_code != function_code_t::read_input_registers &&
        function_code != function_code_t::read_holding_registers) {
        co_return tcp_data_unit(
            transaction_id,
            exception_response(unit_id, function_code,
                               exception_code_t::illegal_function));
    }

    print_logging_handler(log_level::trace,
                          fmt::format("Waiting {}ms", SERVER_WAIT.count()));
    // co_await timer.async_wait(SERVER_WAIT);
    co_await wait_for(SERVER_WAIT);

    if (function_code == function_code_t::read_holding_registers) {

        auto optionalRequest =
            requestDataUnit.pdu<read_holding_registers_request>();

        // If the function code and the request do not agree, co_return an
        // error
        if (!optionalRequest) {
            co_return tcp_data_unit(
                transaction_id,
                exception_response(unit_id, function_code,
                                   exception_code_t::server_device_failure));
        }

        auto request = optionalRequest.value();
        auto requestBitmap =
            create_request_bit_map(request.start_address, request.length);

        // Check if any requested addresses are illegal
        if (!legal_address(validAddresses, requestBitmap,
                           request.start_address)) {
            co_return tcp_data_unit(
                transaction_id,
                exception_response(unit_id, function_code,
                                   exception_code_t::illegal_data_address));
        }

        inputs = copy_data_registers(holding_registers, request.start_address,
                                     request.length);

        co_return tcp_data_unit(
            transaction_id,
            read_holding_registers_response(unit_id, std::move(inputs)));
    } else if (function_code == function_code_t::read_input_registers) {

        auto optionalRequest =
            requestDataUnit.pdu<read_input_registers_request>();

        // If the function code and the request do not agree, co_return an
        // error
        if (!optionalRequest) {
            co_return tcp_data_unit(
                transaction_id,
                exception_response(unit_id, function_code,
                                   exception_code_t::server_device_failure));
        }

        auto request = optionalRequest.value();
        auto requestBitmap =
            create_request_bit_map(request.start_address, request.length);

        // Check if any requested addresses are illegal
        if (!legal_address(validAddresses, requestBitmap,
                           request.start_address)) {
            co_return tcp_data_unit(
                transaction_id,
                exception_response(unit_id, function_code,
                                   exception_code_t::illegal_data_address));
        }

        inputs = copy_data_registers(input_registers, request.start_address,
                                     request.length);

        co_return tcp_data_unit(
            transaction_id,
            read_input_registers_response(unit_id, std::move(inputs)));
    }

    co_return tcp_data_unit(
        transaction_id,
        exception_response(unit_id, function_code,
                           exception_code_t::server_device_failure));
}

awaitable<void> run_invalid_coil_tests(tcp_client& client) {
    uint8_t unit_id = 1;
    uint16_t start_address = 16;
    uint16_t num_read_coils = 5;

    auto [response, error] = co_await client.send_request(client.create_request(
        read_coils_request{unit_id, start_address, num_read_coils}));

    EXPECT_FALSE(error);
    EXPECT_TRUE(response.is_exception());

    auto optionalResponse = response.pdu<exception_response>();
    EXPECT_TRUE(optionalResponse);

    auto exceptionResponse = optionalResponse.value();
    EXPECT_EQ(exceptionResponse.exception_code,
              exception_code_t::illegal_data_address);
}

awaitable<void> run_coil_tests(tcp_client& client) {
    // write coils
    uint8_t unit_id = 1;
    uint16_t start_address = 8;
    uint16_t num_read_coils = 11;
    auto [response, error] = co_await client.send_request(client.create_request(
        write_single_coil_request{unit_id, start_address, coil_status_t::on}));

    EXPECT_FALSE(error);
    EXPECT_FALSE(response.is_exception());

    // read coils
    std::tie(response, error) =
        co_await client.send_request(client.create_request(
            read_coils_request{unit_id, start_address, num_read_coils}));

    EXPECT_FALSE(error);
    EXPECT_FALSE(response.is_exception());

    auto optionalResponse = response.pdu<read_coils_response>();
    EXPECT_TRUE(optionalResponse);

    modbus_response_t data{
        modbus_response_t::data_type(optionalResponse.value().function_code()),
        std::make_shared<buffer_t>(optionalResponse.value().values),
        start_address, (uint16_t)optionalResponse.value().values.size()};

    EXPECT_EQ(data.buffer_length(), num_bytes(num_read_coils));
    EXPECT_EQ(data.getUINT16(0), 0xA901U);
    EXPECT_TRUE(data.getBool(0));
    EXPECT_FALSE(data.getBool(1));
    EXPECT_FALSE(data.getBool(2));
    EXPECT_TRUE(data.getBool(3));
    EXPECT_FALSE(data.getBool(4));
    EXPECT_TRUE(data.getBool(5));
    EXPECT_FALSE(data.getBool(6));
    EXPECT_TRUE(data.getBool(7));
    EXPECT_TRUE(data.getBool(8));
    EXPECT_FALSE(data.getBool(9));
    EXPECT_FALSE(data.getBool(10));
    EXPECT_FALSE(data.getBool(11));
}

awaitable<void> run_invalid_inputs_tests(tcp_client& client) {
    uint8_t unit_id = 1;
    uint16_t start_address = 16;
    uint16_t num_read_inputs = 5;
    auto [response, error] = co_await client.send_request(client.create_request(
        read_discrete_inputs_request{unit_id, start_address, num_read_inputs}));

    EXPECT_FALSE(error);
    EXPECT_TRUE(response.is_exception());

    auto optionalResponse = response.pdu<exception_response>();
    EXPECT_TRUE(optionalResponse);

    auto exceptionResponse = optionalResponse.value();
    EXPECT_EQ(exceptionResponse.exception_code,
              exception_code_t::illegal_data_address);
}

awaitable<void> run_inputs_tests(tcp_client& client) {
    // write inputs
    uint8_t unit_id = 1;
    uint16_t start_address = 8;
    uint16_t num_read_inputs = 11;

    // read coils
    auto [response, error] = co_await client.send_request(client.create_request(
        read_discrete_inputs_request{unit_id, start_address, num_read_inputs}));

    EXPECT_FALSE(error);
    EXPECT_FALSE(response.is_exception());

    auto optionalResponse = response.pdu<read_discrete_inputs_response>();
    EXPECT_TRUE(optionalResponse);

    modbus_response_t data{
        modbus_response_t::data_type(optionalResponse.value().function_code()),
        std::make_shared<buffer_t>(optionalResponse.value().inputs),
        start_address, (uint16_t)optionalResponse.value().inputs.size()};

    EXPECT_EQ(data.buffer_length(), num_bytes(num_read_inputs));
    EXPECT_EQ(data.getUINT16(0), 0x9A06U);
    EXPECT_FALSE(data.getBool(0));
    EXPECT_TRUE(data.getBool(1));
    EXPECT_FALSE(data.getBool(2));
    EXPECT_TRUE(data.getBool(3));
    EXPECT_TRUE(data.getBool(4));
    EXPECT_FALSE(data.getBool(5));
    EXPECT_FALSE(data.getBool(6));
    EXPECT_TRUE(data.getBool(7));
    EXPECT_FALSE(data.getBool(8));
    EXPECT_TRUE(data.getBool(9));
    EXPECT_TRUE(data.getBool(10));
    EXPECT_FALSE(data.getBool(11));
}

awaitable<void> run_invalid_holding_reg_tests(tcp_client& client) {
    uint8_t unit_id = 1;
    uint16_t start_address = 0;
    uint16_t num_read_reg = 4;
    auto [response, error] = co_await client.send_request(client.create_request(
        read_holding_registers_request{unit_id, start_address, num_read_reg}));

    EXPECT_FALSE(error);
    EXPECT_TRUE(response.is_exception());

    auto optionalResponse = response.pdu<exception_response>();
    EXPECT_TRUE(optionalResponse);

    auto exceptionResponse = optionalResponse.value();
    EXPECT_EQ(exceptionResponse.exception_code,
              exception_code_t::illegal_data_address);
}

awaitable<void> run_holding_reg_tests(tcp_client& client) {
    // write reg
    uint8_t unit_id = 1;
    uint16_t start_address = 0;
    uint16_t num_read_reg = 2;

    // read coils
    auto [response, error] = co_await client.send_request(client.create_request(
        read_holding_registers_request{unit_id, start_address, num_read_reg}));

    EXPECT_FALSE(error);
    EXPECT_FALSE(response.is_exception());

    auto optionalResponse = response.pdu<read_holding_registers_response>();
    EXPECT_TRUE(optionalResponse);

    modbus_response_t data{
        modbus_response_t::data_type(optionalResponse.value().function_code()),
        std::make_shared<buffer_t>(optionalResponse.value().values),
        start_address, (uint16_t)optionalResponse.value().values.size()};

    EXPECT_EQ(data.buffer_length(), num_read_reg * 2);
    EXPECT_EQ(data.getUINT16(0), 0x0FF0U);
    EXPECT_EQ(data.getUINT16(1), 0x0FF0U);
}

awaitable<void> run_invalid_input_reg_tests(tcp_client& client) {
    uint8_t unit_id = 1;
    uint16_t start_address = 0;
    uint16_t num_read_reg = 4;
    auto [response, error] = co_await client.send_request(client.create_request(
        read_input_registers_request{unit_id, start_address, num_read_reg}));

    EXPECT_FALSE(error);
    EXPECT_TRUE(response.is_exception());

    auto optionalResponse = response.pdu<exception_response>();
    EXPECT_TRUE(optionalResponse);

    auto exceptionResponse = optionalResponse.value();
    EXPECT_EQ(exceptionResponse.exception_code,
              exception_code_t::illegal_data_address);
}

awaitable<void> run_input_reg_tests(tcp_client& client) {
    // write reg
    uint8_t unit_id = 1;
    uint16_t start_address = 0;
    uint16_t num_read_reg = 2;

    // read coils
    auto [response, error] = co_await client.send_request(client.create_request(
        read_input_registers_request{unit_id, start_address, num_read_reg}));

    EXPECT_FALSE(error);
    EXPECT_FALSE(response.is_exception());

    auto optionalResponse = response.pdu<read_input_registers_response>();
    EXPECT_TRUE(optionalResponse);

    modbus_response_t data{
        modbus_response_t::data_type(optionalResponse.value().function_code()),
        std::make_shared<buffer_t>(optionalResponse.value().values),
        start_address, (uint16_t)optionalResponse.value().values.size()};

    EXPECT_EQ(data.buffer_length(), num_read_reg * 2);
    EXPECT_EQ(data.getUINT16(0), 0xF00FU);
    EXPECT_EQ(data.getUINT16(1), 0xF00FU);
}

awaitable<void> run_server_test(asio::io_context& ctx, tcp_server& server,
                                tcp_client& client, bool stop_context = true) {

    // wait for server
    co_await wait_for(50ms);

    co_await run_invalid_coil_tests(client);
    co_await run_coil_tests(client);
    co_await run_invalid_inputs_tests(client);
    co_await run_inputs_tests(client);
    co_await run_invalid_holding_reg_tests(client);
    co_await run_holding_reg_tests(client);
    co_await run_invalid_input_reg_tests(client);
    co_await run_input_reg_tests(client);

    if (stop_context) {
        server.stop();
        ctx.stop();
    }
    co_return;
}

awaitable<void> run_slow_server_test(asio::io_context& ctx, tcp_server& server,
                                     tcp_client& client,
                                     bool stop_context = true) {
    // wait for server
    auto server_start_timeout = 50ms;
    print_logging_handler(log_level::debug,
                          fmt::format("waiting {}ms for the server to start",
                                      server_start_timeout.count()));
    co_await wait_for(server_start_timeout);

    // write reg
    uint8_t unit_id = 1;
    uint16_t start_address = 0;
    uint16_t num_read_reg = 2;

    // read input registers
    auto [response, error] = co_await client.send_request(
        client.create_request(
            read_input_registers_request{unit_id, start_address, num_read_reg}),
        SERVER_WAIT - 100ms);

    EXPECT_TRUE(error);
    EXPECT_EQ(error.error_code(), (int)boost::asio::error::timed_out);
    EXPECT_EQ(response.type(), message_type::invalid_pdu_type);

    auto request = client.create_request(
        read_holding_registers_request{unit_id, start_address, num_read_reg});
    std::tie(response, error) = co_await client.send_request(request);
    EXPECT_FALSE(error);
    if (error) {
        print_logging_handler(log_level::error, error.message()),
            SERVER_WAIT + 300ms;
    }
    EXPECT_EQ(request.transaction_id(), response.transaction_id());
    auto optionalResponse = response.pdu<read_holding_registers_response>();
    EXPECT_TRUE(optionalResponse);

    modbus_response_t data{
        modbus_response_t::data_type(optionalResponse.value().function_code()),
        std::make_shared<buffer_t>(optionalResponse.value().values),
        start_address, (uint16_t)optionalResponse.value().values.size()};

    EXPECT_EQ(data.buffer_length(), num_read_reg * 2);
    EXPECT_EQ(data.getUINT16(0), 0x0FF0U);
    EXPECT_EQ(data.getUINT16(1), 0x0FF0U);

    if (stop_context) {
        ctx.stop();
    }
    co_return;
}

TEST(tcp_server_test, server_test) {
    asio::io_context ctx(1);

    auto host = get_env_var(ENV_PORT).value_or(DEFAULT_HOST);
    auto portString = get_env_var(ENV_PORT).value_or(DEFAULT_PORT);
    uint16_t port = std::stoi(portString);

    // start the server
    server_config sconfig =
        server_config{host, port}.set_logging_handler(print_logging_handler);
    request_handler handler;
    tcp_server server(ctx.get_executor(), std::ref(handler), sconfig);
    co_spawn(ctx, server.start(), detached);

    // start the client
    client_config cconfig =
        client_config(host, port).set_logging_handler(print_logging_handler);
    tcp_client client(ctx.get_executor(), cconfig);

    // run the test
    co_spawn(ctx, run_server_test(std::ref(ctx), server, client), detached);

    ctx.run();
}

TEST(tcp_server_test, slow_server_test) {
    asio::io_context server_ctx(1);

    auto host = get_env_var(ENV_SLOW_HOST).value_or(DEFAULT_HOST);
    auto portString = get_env_var(ENV_SLOW_PORT).value_or(DEFAULT_SLOW_PORT);
    uint16_t port = std::stoi(portString);

    // start the server
    server_config sconfig =
        server_config{host, port}.set_logging_handler(print_logging_handler);
    request_handler handler;
    tcp_server server(server_ctx.get_executor(), slow_handler, sconfig);
    co_spawn(server_ctx, server.start(), detached);
    std::jthread server_thread([&]() { server_ctx.run(); });

    // start the client
    asio::io_context client_ctx(1);
    client_config cconfig =
        client_config(host, port).set_logging_handler(print_logging_handler);
    tcp_client client(client_ctx.get_executor(), cconfig);

    // run the test
    co_spawn(client_ctx,
             run_slow_server_test(std::ref(client_ctx), server, client),
             detached);

    client_ctx.run();

    // cleanup the server
    server.stop();
    server_ctx.stop();
    server_thread.join();
}

TEST(tcp_server_test, server_test_multi_thread) {
    int num_threads = 8;
    asio::io_context ctx(num_threads);
    std::vector<std::jthread> threads;

    auto host = get_env_var(ENV_PORT).value_or(DEFAULT_HOST);
    auto portString = get_env_var(ENV_PORT).value_or(DEFAULT_PORT);
    uint16_t port = std::stoi(portString);

    // start the server
    server_config sconfig =
        server_config{host, port}.set_logging_handler(print_logging_handler);
    request_handler handler;
    tcp_server server(ctx.get_executor(), std::ref(handler), sconfig);
    co_spawn(ctx, server.start(), detached);

    // start the client
    client_config cconfig =
        client_config(host, port).set_logging_handler(print_logging_handler);
    tcp_client client(ctx.get_executor(), cconfig);

    // run the test
    for (int i = 0; i < num_threads - 1; i++) {
        co_spawn(ctx, run_server_test(std::ref(ctx), server, client, false),
                 detached);
        threads.push_back(std::jthread([&]() { ctx.run(); }));
    }
    co_spawn(ctx, run_server_test(std::ref(ctx), server, client), detached);

    ctx.run();

    for (auto& thread : threads) {
        thread.join();
    }
}

} // namespace