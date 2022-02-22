#include "client/tcp_client.hpp"
#include "core/modbus_response.hpp"
#include "core/requests.hpp"
#include "core/responses.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {

using namespace modbus;
using namespace std;

const std::string ENV_HOST = "E2E_MODBUS_HOST";
const std::string DEFAULT_HOST = "host.docker.internal";
const std::string ENV_PORT = "E2E_MODBUS_PORT";
const std::string DEFAULT_PORT = "5020";

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

void logMessage(log_level level, string_view message) {
    cout << message << endl;
}

awaitable<void> run_coil_tests(asio::io_context& ctx, tcp_client& client,
                               bool stop_context = true) {

    // write coils
    uint8_t unit_id = 1;
    uint16_t start_address = 256;
    uint16_t num_read_coils = 8;
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
    EXPECT_TRUE(data.getBool(0));

    // write multiple coils
    std::tie(response, error) = co_await client.send_request(
        client.create_request(write_multiple_coils_request{
            unit_id, start_address,
            std::vector<bool>{false, true, false, true}}));

    EXPECT_FALSE(error);
    EXPECT_FALSE(response.is_exception());

    // read coils
    std::tie(response, error) =
        co_await client.send_request(client.create_request(
            read_coils_request{unit_id, start_address, num_read_coils}));

    EXPECT_FALSE(error);
    EXPECT_FALSE(response.is_exception());

    optionalResponse = response.pdu<read_coils_response>();
    EXPECT_TRUE(optionalResponse);

    data = modbus_response_t{
        modbus_response_t::data_type(optionalResponse.value().function_code()),
        std::make_shared<buffer_t>(optionalResponse.value().values),
        start_address, (uint16_t)optionalResponse.value().values.size()};

    EXPECT_EQ(data.buffer_length(), num_bytes(num_read_coils));
    EXPECT_FALSE(data.getBool(0));
    EXPECT_TRUE(data.getBool(1));
    EXPECT_FALSE(data.getBool(2));
    EXPECT_TRUE(data.getBool(3));

    if (stop_context) {
        ctx.stop();
    }
    co_return;
}

awaitable<void> run_inputs_tests(asio::io_context& ctx, tcp_client& client,
                                 bool stop_context = true) {

    uint8_t unit_id = 1;
    uint16_t start_address = 256;
    uint16_t num_read_coils = 8;
    auto [response, error] = co_await client.send_request(client.create_request(
        read_discrete_inputs_request{unit_id, start_address, num_read_coils}));

    EXPECT_FALSE(error);
    EXPECT_FALSE(response.is_exception());

    auto optionalResponse = response.pdu<read_discrete_inputs_response>();
    EXPECT_TRUE(optionalResponse);

    modbus_response_t data{
        modbus_response_t::data_type(optionalResponse.value().function_code()),
        std::make_shared<buffer_t>(optionalResponse.value().inputs),
        start_address, (uint16_t)optionalResponse.value().inputs.size()};

    EXPECT_EQ(data.buffer_length(), num_bytes(num_read_coils));
    EXPECT_FALSE(data.getBool(0));

    // read coils
    std::tie(response, error) = co_await client.send_request(
        client.create_request(read_discrete_inputs_request{
            unit_id, start_address, num_read_coils}));

    EXPECT_FALSE(error);
    EXPECT_FALSE(response.is_exception());

    optionalResponse = response.pdu<read_discrete_inputs_response>();
    EXPECT_TRUE(optionalResponse);

    data = modbus_response_t{
        modbus_response_t::data_type(optionalResponse.value().function_code()),
        std::make_shared<buffer_t>(optionalResponse.value().inputs),
        start_address, (uint16_t)optionalResponse.value().inputs.size()};

    EXPECT_EQ(data.buffer_length(), num_bytes(num_read_coils));
    EXPECT_FALSE(data.getBool(0));
    EXPECT_TRUE(data.getBool(1));
    EXPECT_FALSE(data.getBool(2));
    EXPECT_TRUE(data.getBool(3));

    if (stop_context) {
        ctx.stop();
    }
    co_return;
}

awaitable<void> run_holding_reg_tests(asio::io_context& ctx, tcp_client& client,
                                      bool stop_context = true) {

    uint8_t unit_id = 1;
    uint16_t start_address = 0;
    uint16_t offset = 9;
    uint16_t offset_address = start_address + offset;
    uint16_t num_read_reg = 10;
    uint16_t write_val = 255;
    std::vector<uint16_t> write_vals{0x00, 0x01, 0x02, 0x03, 0x04};

    auto [response, error] = co_await client.send_request(client.create_request(
        write_single_register_request{unit_id, offset_address, write_val}));

    EXPECT_FALSE(error);
    EXPECT_FALSE(response.is_exception());

    // read registers
    std::tie(response, error) = co_await client.send_request(
        client.create_request(read_holding_registers_request{
            unit_id, start_address, num_read_reg}));

    EXPECT_FALSE(error);
    EXPECT_FALSE(response.is_exception());

    auto optionalResponse = response.pdu<read_holding_registers_response>();
    EXPECT_TRUE(optionalResponse);

    modbus_response_t data{
        modbus_response_t::data_type(optionalResponse.value().function_code()),
        std::make_shared<buffer_t>(optionalResponse.value().values),
        start_address, (uint16_t)optionalResponse.value().values.size()};

    EXPECT_EQ(data.buffer_length(), num_read_reg * 2);
    EXPECT_TRUE(data.getUINT16(offset));

    // write multiple registers
    start_address = 20;
    num_read_reg = (uint16_t)write_vals.size();
    std::tie(response, error) = co_await client.send_request(
        client.create_request(write_multiple_registers_request{
            unit_id, start_address, (uint16_t)write_vals.size(), write_vals}));

    EXPECT_FALSE(error);
    EXPECT_FALSE(response.is_exception());

    // read registers
    std::tie(response, error) = co_await client.send_request(
        client.create_request(read_holding_registers_request{
            unit_id, start_address, num_read_reg}));

    EXPECT_FALSE(error);
    EXPECT_FALSE(response.is_exception());

    optionalResponse = response.pdu<read_holding_registers_response>();
    EXPECT_TRUE(optionalResponse);

    data = modbus_response_t{
        modbus_response_t::data_type(optionalResponse.value().function_code()),
        std::make_shared<buffer_t>(optionalResponse.value().values),
        start_address, (uint16_t)optionalResponse.value().values.size()};

    EXPECT_EQ(data.buffer_length(), num_read_reg * 2);
    for (uint16_t i = 0; i < num_read_reg; i++) {
        EXPECT_EQ(i, data.getUINT16(i));
    }

    if (stop_context) {
        ctx.stop();
    }
    co_return;
}

awaitable<void> run_input_reg_tests(asio::io_context& ctx, tcp_client& client,
                                    bool stop_context = true) {
    // read input registers
    uint8_t unit_id = 1;
    uint16_t start_address = 0;
    uint16_t num_read_reg = 10;

    // read registers
    auto [response, error] = co_await client.send_request(client.create_request(
        read_input_registers_request{unit_id, start_address, num_read_reg}));

    EXPECT_FALSE(error);
    EXPECT_FALSE(response.is_exception());

    auto optionalResponse = response.pdu<read_input_registers_response>();
    EXPECT_TRUE(optionalResponse);
    auto data = modbus_response_t{
        modbus_response_t::data_type(optionalResponse.value().function_code()),
        std::make_shared<buffer_t>(optionalResponse.value().values),
        start_address, (uint16_t)optionalResponse.value().values.size()};

    // registers are 16-bit so should be requested length * 2 (16/8)
    EXPECT_EQ(data.buffer_length(), num_read_reg * 2);
    EXPECT_EQ(data.getINT16(0), -31527);
    EXPECT_EQ(data.getUINT16(0), 34009);
    EXPECT_EQ(data.getINT16(9), 50);
    EXPECT_EQ(data.getUINT16(9), 50);

    // do another range
    start_address = 426;
    num_read_reg = 6;
    std::tie(response, error) = co_await client.send_request(
        client.create_request(read_input_registers_request{
            unit_id, start_address, num_read_reg}));

    EXPECT_FALSE(error);
    EXPECT_FALSE(response.is_exception());

    optionalResponse = response.pdu<read_input_registers_response>();
    EXPECT_TRUE(optionalResponse);
    data = modbus_response_t{
        modbus_response_t::data_type(optionalResponse.value().function_code()),
        std::make_shared<buffer_t>(optionalResponse.value().values),
        start_address, (uint16_t)optionalResponse.value().values.size()};

    // registers are 16-bit so should be requested length * 2 (16/8)
    EXPECT_EQ(data.buffer_length(), num_read_reg * 2);
    EXPECT_EQ(data.getINT16(0), 57);
    EXPECT_EQ(data.getUINT16(0), 57);
    EXPECT_EQ(data.getINT16(5), 26);
    EXPECT_EQ(data.getUINT16(5), 26);

    if (stop_context) {
        ctx.stop();
    }
    co_return;
}

awaitable<void> wait_and_stop(asio::io_context& ctx,
                              std::chrono::milliseconds timeout) {
    auto exec = co_await asio::this_coro::executor;
    cpool::timer timer(exec);
    co_await timer.async_wait(timeout);
    ctx.stop();
}

TEST(tcp_client_test, coils) {
    asio::io_context ctx(1);

    auto host = get_env_var(ENV_HOST).value_or(DEFAULT_HOST);
    auto portString = get_env_var(ENV_PORT).value_or(DEFAULT_PORT);
    uint16_t port = std::stoi(portString);

    client_config config =
        client_config(host, port)
            .set_logging_handler(std::bind(logMessage, std::placeholders::_1,
                                           std::placeholders::_2));
    print_logging_handler(
        log_level::debug,
        fmt::format("connecting to {}:{}", config.host, config.port));
    tcp_client client(ctx.get_executor(), config);
    cpool::co_spawn(ctx, run_coil_tests(std::ref(ctx), std::ref(client)),
                    cpool::detached);

    ctx.run();
}

TEST(tcp_client_test, discrete_inputs) {
    asio::io_context ctx(1);

    auto host = get_env_var(ENV_HOST).value_or(DEFAULT_HOST);
    auto portString = get_env_var(ENV_PORT).value_or(DEFAULT_PORT);
    uint16_t port = std::stoi(portString);

    client_config config =
        client_config(host, port)
            .set_logging_handler(std::bind(logMessage, std::placeholders::_1,
                                           std::placeholders::_2));
    tcp_client client(ctx.get_executor(), config);
    cpool::co_spawn(ctx, run_inputs_tests(std::ref(ctx), std::ref(client)),
                    cpool::detached);

    ctx.run();
}

TEST(tcp_client_test, holding_registers) {
    asio::io_context ctx(1);

    auto host = get_env_var(ENV_HOST).value_or(DEFAULT_HOST);
    auto portString = get_env_var(ENV_PORT).value_or(DEFAULT_PORT);
    uint16_t port = std::stoi(portString);

    client_config config =
        client_config(host, port)
            .set_logging_handler(std::bind(logMessage, std::placeholders::_1,
                                           std::placeholders::_2));
    print_logging_handler(
        log_level::debug,
        fmt::format("connecting to {}:{}", config.host, config.port));
    tcp_client client(ctx.get_executor(), config);
    cpool::co_spawn(ctx, run_holding_reg_tests(std::ref(ctx), std::ref(client)),
                    cpool::detached);

    ctx.run();
}

TEST(tcp_client_test, input_registers) {
    asio::io_context ctx(1);

    auto host = get_env_var(ENV_HOST).value_or(DEFAULT_HOST);
    auto portString = get_env_var(ENV_PORT).value_or(DEFAULT_PORT);
    uint16_t port = std::stoi(portString);

    client_config config =
        client_config(host, port)
            .set_logging_handler(std::bind(logMessage, std::placeholders::_1,
                                           std::placeholders::_2));
    print_logging_handler(
        log_level::debug,
        fmt::format("connecting to {}:{}", config.host, config.port));
    tcp_client client(ctx.get_executor(), config);
    cpool::co_spawn(ctx, run_input_reg_tests(std::ref(ctx), std::ref(client)),
                    cpool::detached);

    ctx.run();
}

TEST(tcp_client_test, multi_test) {
    asio::io_context ctx(8);

    auto host = get_env_var(ENV_HOST).value_or(DEFAULT_HOST);
    auto portString = get_env_var(ENV_PORT).value_or(DEFAULT_PORT);
    uint16_t port = std::stoi(portString);

    client_config config =
        client_config(host, port)
            .set_logging_handler(std::bind(logMessage, std::placeholders::_1,
                                           std::placeholders::_2));
    print_logging_handler(
        log_level::debug,
        fmt::format("connecting to {}:{}", config.host, config.port));
    tcp_client client(ctx.get_executor(), config);

    // run several test simultaneously
    cpool::co_spawn(ctx, run_coil_tests(std::ref(ctx), std::ref(client), false),
                    cpool::detached);
    cpool::co_spawn(ctx,
                    run_inputs_tests(std::ref(ctx), std::ref(client), false),
                    cpool::detached);
    cpool::co_spawn(
        ctx, run_holding_reg_tests(std::ref(ctx), std::ref(client), false),
        cpool::detached);
    cpool::co_spawn(ctx,
                    run_input_reg_tests(std::ref(ctx), std::ref(client), false),
                    cpool::detached);
    cpool::co_spawn(
        ctx, wait_and_stop(std::ref(ctx), std::chrono::milliseconds(300)),
        cpool::detached);

    ctx.run();
}

} // namespace