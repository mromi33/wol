#pragma once
#include <asio.hpp>
#include <asio/awaitable.hpp>
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/detail/chrono.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/icmp.hpp>
#include <asio/steady_timer.hpp>
#include <asio/streambuf.hpp>
#include <asio/this_coro.hpp>
#include <asio/use_awaitable.hpp>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <istream>
#include <ostream>
#include <string_view>

#include "icmp_header.hpp"
#include "ipv4_header.hpp"

using asio::awaitable;
using asio::co_spawn;
using asio::ip::icmp;

class ping_local {

    public:
        ping_local(asio::io_context &io);

    protected:
        static auto get_proc_ident() -> std::uint16_t;

        auto start_ping(std::string_view body, std::string_view ip) -> void;

        auto send() -> awaitable<void>;

        auto handle_exp() -> awaitable<void>;

        auto receive() -> awaitable<void>;

        auto handle_rec(std::uint16_t length) -> void;

        icmp_header _rep_icmp_h;
        ipv4_header _rep_ipv4_h;

        asio::io_context &_io;
        asio::chrono::steady_clock::time_point _send_t;
        asio::steady_timer _t;
        asio::streambuf _rec_buf;

        icmp::resolver _r;
        icmp::endpoint _e;
        icmp::socket _s;

        std::uint16_t _sequence_num;
        std::string _body;
        std::uint16_t _num_rep;
};