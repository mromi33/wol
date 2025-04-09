#include "core.hpp"
#include <asio/awaitable.hpp>
#include <asio/buffer.hpp>
#include <asio/detached.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/udp.hpp>
#include <asio/socket_base.hpp>
#include <asio/system_error.hpp>
#include <asio/use_awaitable.hpp>
#include <cctype>
#include <exception>
#include <ios>
#include <memory>
#include <stdexcept>
#include "ping.hpp"

using asio::ip::udp;

wol::wol(asio::io_context &io) : _io(io), _socket(io, udp::v4()) {
}


bool wol::create_magic_packet(std::string_view mac_addr) {

    if (mac_addr.size() != 17) return false;

    _m_p.resize(102);

    std::fill_n(_m_p.begin(), 6, 0xFF);

    unsigned char mac[6];

    sscanf(mac_addr.data(), "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
           &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);

    for (int i = 0; i < 16; ++i) {

        std::copy_n(mac, 6, _m_p.begin() + 6 + i * 6);
    }
    return true;
}

auto wol::send_magic_packet(std::string_view mac_addr,
                            std::string_view ip) -> awaitable<bool> {

    if (!create_magic_packet(mac_addr)) co_return false;

    if (_m_p.size() != 102) {
        std::cout << "Magic Packet has wrong size: " << _m_p.size() << "\n";
        co_return false;
    }

    try {

        udp::resolver res(_io);

        auto e = res.resolve(udp::v4(), "255.255.255.255", "9");

        if (e.empty()) co_return false;

        _socket.set_option(asio::socket_base::broadcast(true));

        co_await _socket.async_send_to(asio::buffer(_m_p), *e.begin(),
                                       asio::deferred);

        co_return true;

    } catch (const std::exception &exc) {

        std::cout << "Something has happened: " << exc.what() << '\n';

        co_return false;
    }
}
