#ifndef MOSAIC_GNSS_DRIVER_SBF_H
#define MOSAIC_GNSS_DRIVER_SBF_H

#include <fstream>

namespace sbf {
    class SBF {
        static constexpr const uint8_t sync_chars[] = {'$', '@'};

        // Data Storages
        static const size_t buffer_size = 256;
        char _buffer[buffer_size]; // Any point in std::array ?
        uint8_t *const buffer = reinterpret_cast<uint8_t *const>(_buffer);
        size_t buffer_use;

        const uint8_t *data_start;
        size_t data_length;

        //
        const uint8_t *read_ptr;
        const uint8_t *block_start;

        const uint8_t *read(size_t size);

        bool seek_block();

        // Helper functions for read()
        bool in_buffer(const uint8_t *const ptr) {
            return buffer <= ptr && ptr < buffer + buffer_size;
        }

        bool in_data(const uint8_t *const ptr) {
            return data_start <= ptr && ptr < data_start + data_length;
        }

    public:
        SBF();

        void parse(const uint8_t *data, size_t size);

        bool parse_block();
    };
}

#endif //MOSAIC_GNSS_DRIVER_SBF_H
