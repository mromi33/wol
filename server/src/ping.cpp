#include "ping.hpp"
#include <asio/error.hpp>
#include <asio/error_code.hpp>
#include <asio/steady_timer.hpp>
#include <asio/system_error.hpp>
#include <asio/use_awaitable.hpp>
#include <cstdint>
#include <string>
#include <string_view>

ping_local::ping_local(asio::io_context &io)
    : _io(io), _r(io), _t(io), _s(io, icmp::v4()), _sequence_num(0) {};

auto ping_local::get_proc_ident() -> std::uint16_t {

#if defined(ASIO_WINDOWS)
    return static_cast<std::uint16_t>(::GetCurrentProcessId());
#else
    return static_cast<unsigned short>(::getpid());
#endif
}

auto ping_local::start_ping(std::string_view body,
                            std::string_view ip) -> void {

    _body = body;

    _e = *_r.resolve(icmp::v4(), std::string(ip), "");

    co_spawn(_io, send(), asio::detached);
    co_spawn(_io, receive(), asio::detached);
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

    co_await _s.async_send_to(buff.data(), _e, asio::use_awaitable);

    _num_rep = 0;

    try {

        _t.expires_after(asio::chrono::seconds(5));

        co_await _t.async_wait(asio::deferred);

        co_spawn(_io, handle_exp(), asio::detached);

    } catch (const asio::system_error &exc) {

        if (exc.code() != asio::error::operation_aborted)
            std::cout << "Sending error: " << exc.what() << "\n";
    }
}

auto ping_local::handle_exp() -> awaitable<void> {

    try {

        if (_num_rep == 0) {

            _t.expires_after(asio::chrono::seconds(1));

            co_await _t.async_wait(asio::deferred);
            co_spawn(_io, send(), asio::detached);
        }

    } catch (const std::exception &exc) {

        std::cout << "Something has happened while expiration handling: "
                  << exc.what() << '\n';
    }
}

auto ping_local::receive() -> awaitable<void> {

    auto length = co_await _s.async_receive(_rec_buf.prepare(65536),
                                            asio::deferred);

    handle_rec(length);

    co_spawn(_io, send(), asio::detached);
}

auto ping_local::handle_rec(std::uint16_t length) -> void {

    _rec_buf.commit(length);

    std::istream is(&_rec_buf);

    is >> _rep_ipv4_h >> _rep_icmp_h;

    if (is && _rep_icmp_h.type() == icmp_header::echo_reply &&
        _rep_icmp_h.identifier() == get_proc_ident() &&
        _rep_icmp_h.sequence_num() == _sequence_num) {

        if (_num_rep++ == 0) _t.cancel();

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
    }

    _rec_buf.consume(_rec_buf.size());
}
