#include "ping.hpp"
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/io_context.hpp>
#include <asio/use_awaitable.hpp>
#include <core.hpp>

int main() {

    asio::io_context io;

    core core(io, "30:9c:23:a6:80:5a", "192.168.1.131");
    co_spawn(io, core.launching_device(), asio::detached);
    core.run();

    return 0;
}