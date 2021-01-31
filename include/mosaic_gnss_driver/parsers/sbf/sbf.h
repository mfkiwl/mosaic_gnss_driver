#ifndef MOSAIC_GNSS_DRIVER_SBF_H
#define MOSAIC_GNSS_DRIVER_SBF_H

#include <mosaic_gnss_driver/data_buffers.h>
#include <mosaic_gnss_driver/parsers/sbf/block_parsers.h>

#include <unordered_map>

namespace sbf
{
    /**
     * Using this class:
     *     - Construct and object of the class. This also initializes the various parsers.
     *     - Whenever you receive data, call `parse` with the buffer storing the data.
     *     - Use the parsed data in the DataBuffer
     *
     * Adding more SBF Blocks:
     *     - `seek`, `read`, and `unread` are essentially black boxes and the implementation should not matter.
     *     - To add a parser: define it in block_parsers.h, create it in the struct parsers, and add it to parse_table.
     */

    class SBF
    {

        // Reading Data:

        /// The string demarcating start of block.
        static constexpr const uint8_t sync_chars[2] = {'$', '@'};

        /// Internal Buffer
        static const size_t buffer_size = 256; // Approximately max message size
        char _buffer[buffer_size] = {0};       // Any point in std::array ?
        uint8_t *const buffer = reinterpret_cast<uint8_t *const>(_buffer);
        size_t buffer_use{0};

        /// Received Data
        const uint8_t *data_start{nullptr};
        const uint8_t *data_end{nullptr};

        /// Next data to read
        const uint8_t *read_ptr{nullptr}; // Current position in buffer + data
        /// Start of block being parsed
        const uint8_t *block_start{nullptr};

        mosaic_gnss_driver::DataBuffers &data_buf;

        // Parsing Blocks:
        using parse_table_t = std::unordered_map<sbf::u4, std::function<void(const uint8_t *, const sbf::u2,
                                                                             const sbf::u1)>>;
        parse_table_t parse_table{};

        /**
         * Parses the block starting at `block_start`
         *
         * @return Whether data is exhausted
         */
        bool parse_block();

        bool in_buffer(const uint8_t *const ptr)
        {
            return buffer <= ptr && ptr < buffer + buffer_size;
        }

        bool in_data(const uint8_t *const ptr)
        {
            return data_start <= ptr && ptr < data_end;
        }

        /**
         * Seeks until sync str of block found, `sync_chars`
         *
         * Sets `block_start` and `read_ptr` to the location where block is found.
         *
         * If no more bytes to be read, clears the internal buffer and returns false.
         * If last byte is the first sync char, copies it to the internal buffer and returns false.
         *
         * @return true on block found, false on end of data
         */
        bool seek_block();

        /**
         * When inside a block, reads bytes. Ensures the read bytes are stored contiguously with the rest of the block.
         * requires `block_start` to be set.
         *
         * If no more bytes to read, copies partially read block to buffer, and returns nullptr
         *
         * @param size number of bytes to read
         * @return pointer to the location of the read bytes, nullptr if data exhausted
         */
        const uint8_t *read(size_t size);

        /**
         * Moves the read pointer backwards.
         * Used when a block turns out to be invalid.
         * @param rewind_len
         */
        void unread(size_t rewind_len);

        /**
         * Performs CRC check according to the SBF specification
         *
         * TODO: Implement
         *
         * @param bytes : Start of bytes to be CRC checked
         * @param length  : Number of bytes to be CRC checked
         * @param crc : The correct CRC value
         * @return : Pass/Fail
         */
        static bool check_crc(const uint8_t *bytes, size_t length, uint16_t crc);

    public:
        explicit SBF(mosaic_gnss_driver::DataBuffers &buffers);

        /**
         * Searches for and parses SBF blocks.
         *
         * @param data Points to the new set of data
         * @param size Size of the new set of data
         */
        void parse(const uint8_t *data, size_t size);

        struct _Parsers
        {
            mosaic_gnss_driver::DataBuffers &data_buf;
            parse_table_t &pt;

            sbf::block_parsers::Geodetic geodetic{data_buf};
            sbf::block_parsers::Cartesian cartesian{data_buf};

            _Parsers(mosaic_gnss_driver::DataBuffers &dbuff, parse_table_t &_pt) : data_buf(dbuff), pt(_pt)
            {
            }

            void enable_geodetic()
            {
                data_buf.nav_sat_fix.enabled = true;
                data_buf.velocity.enabled = true;

                pt[4007] = [&](const uint8_t *block_ptr, const sbf::u2 len, const sbf::u1 rev_num) { geodetic.PVTGeodetic(block_ptr, len, rev_num); };
                pt[5906] = [&](const uint8_t *block_ptr, const sbf::u2 len, const sbf::u1 rev_num) { geodetic.PosCovGeodetic(block_ptr, len, rev_num); };
                pt[5908] = [&](const uint8_t *block_ptr, const sbf::u2 len, const sbf::u1 rev_num) { geodetic.VelCovGeodetic(block_ptr, len, rev_num); };
            }

            void enable_cartesian()
            {
                data_buf.pose.enabled = true;
                data_buf.velocity.enabled = true;

                pt[4006] = [&](const uint8_t *block_ptr, const sbf::u2 len, const sbf::u1 rev_num) { cartesian.PVTCartesian(block_ptr, len, rev_num); };
                pt[5905] = [&](const uint8_t *block_ptr, const sbf::u2 len, const sbf::u1 rev_num) { cartesian.PosCovCartesian(block_ptr, len, rev_num); };
                pt[5907] = [&](const uint8_t *block_ptr, const sbf::u2 len, const sbf::u1 rev_num) { cartesian.VelCovCartesian(block_ptr, len, rev_num); };
            }

        } parsers{data_buf, parse_table};
    };
} // namespace sbf

#endif //MOSAIC_GNSS_DRIVER_SBF_H
