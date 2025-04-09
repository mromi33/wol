#include <asio/awaitable.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/udp.hpp>
#include <cstdint>
#include <memory>
#include <vector>
#include "ping.hpp"

using asio::ip::udp;

class wol {

    public:
        wol(asio::io_context &io);

        auto create_magic_packet(std::string_view m_addr) -> bool;

        auto send_magic_packet(std::string_view m_addr,
                               std::string_view ip) -> awaitable<bool>;

    private:
        asio::io_context &_io;

        udp::socket _socket;

        std::vector<unsigned char> _m_p;
};