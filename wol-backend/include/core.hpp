#include <asio/io_context.hpp>
#include <memory>
#include "ping.hpp"
class wol {

    public:
        wol();

        auto run() -> void;

    private:
        std::shared_ptr<ping_local> _ping;
        asio::io_context _io;
};