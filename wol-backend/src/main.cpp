#include "ping.hpp"
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/io_context.hpp>
#include <asio/use_awaitable.hpp>
#include <core.hpp>

int main(int argc, char *argv[]) {

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <mac_address> <ip_address>\n";
        return 1;
    }

    asio::io_context io;

    core core(io, argv[argc - 2], argv[argc - 1]);

    co_spawn(io, core.launching_device(), asio::detached);
    core.run();

    return 0;
}