#pragma once
#include <algorithm>
#include <asio/ip/address_v4.hpp>
#include <cstdint>
#include <ios>
#include <istream>

class ipv4_header {

    public:
        ipv4_header();

        auto ip_version() const -> std::uint8_t;

        auto header_length() const -> std::uint8_t;

        auto type_of_service() const -> std::uint16_t;

        auto total_length_bytes() const -> std::uint16_t;

        auto identification() const -> std::uint16_t;

        auto df_check() const -> bool;

        auto mf_check() const -> bool;

        auto f_offset() const -> std::uint16_t;

        auto ttl() const -> std::uint32_t;

        auto protocol() const -> std::uint16_t;

        auto header_checksum() const -> std::uint16_t;

        auto source_ip() const -> asio::ip::address_v4;

        auto destination_ip() const -> asio::ip::address_v4;

        friend auto operator>>(std::istream &is,
                               ipv4_header &h) -> std::istream & {

            is.read(reinterpret_cast<char *>(h._data), 20);

            if (h.ip_version() != 4) is.setstate(std::ios::failbit);

            std::streamsize options_length = h.header_length() - 20;

            if (options_length < 0 || options_length > 40)
                is.setstate(std::ios::failbit);

            else {
                is.read(reinterpret_cast<char *>(h._data) + 20, options_length);
            }

            return is;
        }

    protected:
        auto decode(std::int32_t a, std::int32_t b) const -> std::uint16_t;

        unsigned char _data[60];
};