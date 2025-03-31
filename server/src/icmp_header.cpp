#include "icmp_header.hpp"

icmp_header::icmp_header() {
    std::fill(_data, _data + sizeof(_data), 0);
}

auto icmp_header::type() const -> std::uint16_t {
    return _data[0];
}

auto icmp_header::code() const -> std::uint16_t {
    return _data[1];
}

auto icmp_header::check_sum() const -> std::uint16_t {
    return decode(2, 3);
}

auto icmp_header::identifier() const -> std::uint16_t {

    return decode(4, 5);
}

auto icmp_header::sequence_num() const -> std::uint16_t {

    return decode(6, 7);
}

auto icmp_header::type(std::uint16_t d) -> void {
    _data[0] = d;
}

auto icmp_header::code(std::uint16_t d) -> void {
    _data[1] = d;
}

auto icmp_header::check_sum(std::uint16_t d) -> void {

    encode(2, 3, d);
}

auto icmp_header::identifier(std::uint16_t d) -> void {

    encode(4, 5, d);
}

auto icmp_header::sequence_num(std::uint16_t d) -> void {

    encode(6, 7, d);
}

auto icmp_header::decode(int a, int b) const -> std::uint16_t {

    return (_data[a] << 8) + _data[b];
}

auto icmp_header::encode(int a, int b, std::uint16_t d) -> void {

    _data[a] = static_cast<std::uint8_t>(d >> 8);
    _data[b] = static_cast<std::uint8_t>(d & 0xFF);
}
