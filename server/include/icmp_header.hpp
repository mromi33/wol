#include <algorithm>
#include <cstdint>
#include <fstream>
#include <istream>
#include <iterator>
#include <ostream>
class icmp_header {

    public:
        enum {
            echo_reply = 0,
            destination_unreachable = 3,
            source_quench = 4,
            redirect = 5,
            echo_request = 8,
            time_exceeded = 11,
            parameter_problem = 12,
            timestamp_request = 13,
            timestamp_reply = 14,
            info_request = 15,
            info_reply = 16,
            address_request = 17,
            address_reply = 18
        };

        icmp_header();

        auto type() const -> std::uint16_t;

        auto code() const -> std::uint16_t;

        auto check_sum() const -> std::uint16_t;

        auto identifier() const -> std::uint16_t;

        auto sequence_num() const -> std::uint16_t;

        auto type(std::uint16_t d) -> void;

        auto code(std::uint16_t d) -> void;

        auto check_sum(std::uint16_t d) -> void;

        auto identifier(std::uint16_t d) -> void;

        auto sequence_num(std::uint16_t d) -> void;

        inline friend auto operator>>(std::istream &is,
                                      icmp_header &icmp_h) -> std::istream & {

            return is.read(reinterpret_cast<char *>(icmp_h._data), 8);
        };

        inline friend auto operator<<(std::ostream &os,
                                      icmp_header &icmp_h) -> std::ostream & {

            return os.write(reinterpret_cast<char *>(icmp_h._data), 8);
        }

    protected:
        std::uint8_t _data[8];

        auto decode(int a, int b) const -> std::uint16_t;

        auto encode(int a, int b, std::uint16_t d) -> void;
};

template <std::contiguous_iterator Iterator>
auto make_checksum(icmp_header &icmp_h, Iterator begin, Iterator end) -> void {

    auto sum = (icmp_h.type() << 8) + icmp_h.code() + icmp_h.identifier() +
               icmp_h.sequence_num();

    Iterator body_iter = begin;


    while (body_iter != end) {

        sum += static_cast<std::uint8_t>(*body_iter++) << 8;

        if (body_iter != end) sum += static_cast<std::uint8_t>(*body_iter++);
    }

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);

    icmp_h.check_sum(static_cast<std::uint16_t>(~sum));
}