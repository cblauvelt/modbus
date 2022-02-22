# modbus
A library that implements the modbus protocol

## How to Use
modbus depends on the [connection-pool library](https://github.com/cblauvelt/connection-pool). Install the conan dependencies

### Get Dependencies
If you have docker installed you can:
```bash
docker volume create conan
git clone https://github.com/cblauvelt/connection-pool.git
docker container run --rm -v conan:/home/conan/.conan -v $(pwd)/connection-pool:/src cblauvelt/cpp-builder-gcc10:latest conan create /src
```

If you have conan installed in your system and a compiler that supports C++20:
```bash
git clone https://github.com/cblauvelt/connection-pool.git
cd connection-pool
conan create .
```

### Create Dependency
If you have docker installed you can:
```bash
docker volume create conan
git clone https://github.com/cblauvelt/modbus.git
docker container run --rm -v conan:/home/conan/.conan -v $(pwd)/modbus:/src cblauvelt/cpp-builder-gcc10:latest conan create /src
```

If you have conan installed in your system and a compiler that supports C++20:
```bash
git clone https://github.com/cblauvelt/modbus.git
cd modbus
conan create .
```

If you're not using conan, you can simply copy the include files into your project.


### Using VSCode
If you're using VSCode you can now use the library in your project by following the instructions here:
https://github.com/cblauvelt/docker-vscode-cpp.git

## Examples
Ensure that you start a modbus server before running this example.

Using the modbus client:
```C++
#include "client/tcp_client.hpp"
#include "core/modbus_response.hpp"
#include "core/requests.hpp"
#include "core/responses.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {

using namespace modbus;

const std::string ENV_HOST = "MODBUS_HOST";
const std::string DEFAULT_HOST = "127.0.0.1";
const std::string ENV_PORT = "MODBUS_PORT";
const std::string DEFAULT_PORT = "502";

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

awaitable<void> run_coil_tests(asio::io_context& ctx, tcp_client& client) {

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

    ctx.stop();
    
    co_return;
}

TEST(tcp_client_test, basic_test) {
    asio::io_context ctx(1);

    auto host = get_env_var(ENV_HOST).value_or(DEFAULT_HOST);
    auto portString = get_env_var(ENV_PORT).value_or(DEFAULT_PORT);
    uint16_t port = std::stoi(portString);

    client_config config =
        client_config(host, port)
            .set_logging_handler(print_logging_handler);
    print_logging_handler(
        log_level::info,
        fmt::format("connecting to {}:{}", config.host, config.port));
    tcp_client client(ctx.get_executor(), config);
    cpool::co_spawn(ctx, run_coil_tests(std::ref(ctx), std::ref(client)),
                    cpool::detached);

    ctx.run();
}

} // namespace

```