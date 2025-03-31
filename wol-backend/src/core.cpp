#include "core.hpp"
#include <asio/io_context.hpp>
#include <memory>
#include "ping.hpp"

wol::wol() {

    _ping = std::make_shared<ping_local>(_io);
}

auto wol::run() -> void {

    _io.run();
}