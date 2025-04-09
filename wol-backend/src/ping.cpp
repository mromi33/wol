#include "ping.hpp"
#include <asio/awaitable.hpp>
#include <asio/co_spawn.hpp>
#include <asio/coroutine.hpp>
#include <asio/detached.hpp>
#include <asio/detail/chrono.hpp>
#include <asio/error.hpp>
#include <asio/error_code.hpp>
#include <asio/experimental/cancellation_condition.hpp>
#include <asio/experimental/coro.hpp>
#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>
#include <asio/system_error.hpp>
#include <asio/use_awaitable.hpp>
#include <asio/use_future.hpp>
#include <cstdint>
#include <exception>
#include <string>
#include <string_view>
#include <asio/experimental/use_coro.hpp>
#include <variant>
#include "asio/experimental/parallel_group.hpp"
#include "asio/experimental/awaitable_operators.hpp"

using namespace asio::experimental::awaitable_operators;

ping_local::ping_local(asio::io_context &io)
    : _io(io), _r(io), _s(io, icmp::v4()), _sequence_num(0) {};

auto ping_local::get_proc_ident() -> std::uint16_t {

#if defined(ASIO_WINDOWS)
    return static_cast<std::uint16_t>(::GetCurrentProcessId());
#else
    return static_cast<unsigned short>(::getpid());
#endif
}

auto ping_local::start_ping(std::string_view body,
                            std::string_view ip) -> awaitable<bool> {

    _body = body;

    auto res = co_await _r.async_resolve(icmp::v4(), std::string(ip), "",
                                         asio::use_awaitable);

    _e = *res.begin();

    auto p_res = co_await (receive() && send());

    co_return false;
}

auto ping_local::send() -> awaitable<void> {

    icmp_header req;
    req.type(icmp_header::echo_request);
    req.code(0);
    req.identifier(get_proc_ident());
    req.sequence_num(++_sequence_num);
    make_checksum(req, _body.begin(), _body.end());

    asio::streambuf buff;
    std::ostream os(&buff);

    os << req << _body;

    _send_t = asio::steady_timer::clock_type::now();

    _num_rep = 0;

    try {

        auto send = co_await _s.async_send_to(buff.data(), _e,
                                              asio::use_awaitable);

    } catch (const asio::system_error &exc) {

        if (exc.code() != asio::error::operation_aborted)
            std::cout << "Sending error: " << exc.what() << "\n";
    }
}

auto ping_local::receive() -> awaitable<bool> {

    _rec_buf.consume(_rec_buf.size());

    asio::steady_timer t(_io);
    t.expires_after(asio::chrono::seconds(5));

    try {

        t.async_wait([this](const asio::error_code &err) {
            if (err != asio::error::operation_aborted) _s.cancel();
        });

        auto length = co_await _s.async_receive(_rec_buf.prepare(65536),
                                                asio::deferred);

        auto rep = co_await handle_rec(length);

        co_return rep;

    } catch (const std::exception &exc) {

        std::cout << "Something has happened with receiving process: "
                  << exc.what() << '\n';

        co_return false;
    }
}

auto ping_local::handle_rec(std::uint16_t length) -> awaitable<bool> {

    _rec_buf.commit(length);

    std::istream is(&_rec_buf);

    is >> _rep_ipv4_h >> _rep_icmp_h;

    if (is && _rep_icmp_h.type() == icmp_header::echo_reply &&
        _rep_icmp_h.identifier() == get_proc_ident() &&
        _rep_icmp_h.sequence_num() == _sequence_num) {

        auto time_now = asio::chrono::steady_clock::now();
        auto sd_time = time_now - _send_t;

        std::cout
            << "---------------------IPv4-packet---------------------" << '\n'
            << "Sending time: "
            << std::chrono::duration_cast<std::chrono::seconds>(
                   _send_t.time_since_epoch())
                   .count()
            << '\n'
            << "Receiving time: "
            << std::chrono::duration_cast<std::chrono::seconds>(sd_time).count()
            << '\n'
            << "TOS: " << _rep_ipv4_h.type_of_service() << '\n'
            << "Total length (in bytes): " << _rep_ipv4_h.total_length_bytes()
            << '\n'
            << "Id of ipv4-packet: " << _rep_ipv4_h.identification() << '\n'
            << "Do not fragment: " << _rep_ipv4_h.df_check() << ' '
            << "More fragment: " << _rep_ipv4_h.mf_check() << ' '
            << "Fragment offset: " << _rep_ipv4_h.f_offset() << '\n'
            << "TTL: " << _rep_ipv4_h.ttl() << '\n'
            << "Protocol: " << _rep_ipv4_h.protocol() << '\n'
            << "From: " << _rep_ipv4_h.source_ip() << ' '
            << "To: " << _rep_ipv4_h.destination_ip() << '\n'
            << "---------------------ICMP-packet---------------------" << '\n'
            << "Type: " << _rep_icmp_h.type() << '\n'
            << "Check sum of packet: " << _rep_icmp_h.check_sum() << '\n'
            << "Code of type: " << _rep_icmp_h.code() << '\n'
            << "Identifier of process: " << _rep_icmp_h.identifier() << '\n'
            << "Sequence num of packet: " << _rep_icmp_h.sequence_num() << '\n';

        co_return true;
    }

    else {

        co_return false;
    }
}
