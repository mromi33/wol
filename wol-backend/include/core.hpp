#pragma once

#include <asio/awaitable.hpp>
#include <asio/io_context.hpp>
#include <memory>
#include <string>
#include "ping.hpp"
#include "wol.hpp"

class core {

    public:
        core(asio::io_context &io, std::string_view mac_addr,
             std::string_view ip);

        auto scan_net() -> void;

        auto launching_device() -> awaitable<bool>;

        auto run() -> void;

    protected:
        asio::io_context &_io;

        std::string _mac_addr;
        std::string _ip;

        std::shared_ptr<wol> _wol;
        std::shared_ptr<ping_local> _ping;
};