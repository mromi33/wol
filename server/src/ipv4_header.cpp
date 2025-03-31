#include "ipv4_header.hpp"

ipv4_header::ipv4_header() {

    std::fill(_data, _data + sizeof(_data), 0);
}

auto ipv4_header::ip_version() const -> std::uint8_t {
    return (_data[0] >> 4) & 0xF;
}

auto ipv4_header::header_length() const -> std::uint8_t {
    return (_data[0] & 0xF) * 4;
}

auto ipv4_header::type_of_service() const -> std::uint16_t {

    return _data[1];
};

auto ipv4_header::total_length_bytes() const -> std::uint16_t {
    return decode(2, 3);
};

auto ipv4_header::identification() const -> std::uint16_t {
    return decode(4, 5);
}

auto ipv4_header::df_check() const -> bool {

    return (_data[6] & 0x40) != 0;
}

auto ipv4_header::mf_check() const -> bool {

    return (_data[6] & 0x20) != 0;
}

auto ipv4_header::f_offset() const -> std::uint16_t {
    return decode(6, 7) & 0x1FFF;
}

auto ipv4_header::ttl() const -> std::uint32_t {
    return _data[8];
};

auto ipv4_header::protocol() const -> std::uint16_t {

    return _data[9];
};

auto ipv4_header::header_checksum() const -> std::uint16_t {

    return decode(10, 11);
};

auto ipv4_header::source_ip() const -> asio::ip::address_v4 {

    asio::ip::address_v4::bytes_type b_add = {

        {_data[12], _data[13], _data[14], _data[15]}};

    return asio::ip::address_v4(b_add);
}

auto ipv4_header::destination_ip() const -> asio::ip::address_v4 {

    asio::ip::address_v4::bytes_type b_add = {

        {_data[16], _data[17], _data[18], _data[19]}};

    return asio::ip::address_v4(b_add);
}

auto ipv4_header::decode(std::int32_t a,
                         std::int32_t b) const -> std::uint16_t {

    return (_data[a] << 8) + _data[b];
}