#include "core.hpp"
#include "ping.hpp"
#include <asio/awaitable.hpp>
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/detail/chrono.hpp>
#include <asio/io_context.hpp>
#include <asio/use_future.hpp>
#include <memory>

core::core(asio::io_context &io, std::string_view mac_addr, std::string_view ip)
    : _io(io), _mac_addr(mac_addr), _ip(ip) {

    _wol = std::make_shared<wol>(io);
    _ping = std::make_shared<ping_local>(io);
}

auto core::launching_device() -> awaitable<bool> {

    asio::steady_timer timer(co_await asio::this_coro::executor);
    timer.expires_after(asio::chrono::seconds(1));
    try {

        auto p_r = co_await _ping->start_ping("", _ip);

        if (!p_r) {

            std::cout << p_r << " ping return\n";

            std::cout << "Sending m packet\n";

            auto f2 = co_await _wol->send_magic_packet(_mac_addr, _ip);

            co_return true;
        }

        co_return false;

    } catch (const std::exception &exc) {

        std::cout << "Something happened with coroutines: 'start_ping' or "
                     "'send_magic_packet'?"
                  << exc.what() << '\n';

        co_return false;
    }
}

auto core::run() -> void {

    _io.run();
}