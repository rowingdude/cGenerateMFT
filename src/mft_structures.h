#ifndef MFT_STRUCTURES_H
#define MFT_STRUCTURES_H

#include <stdint.h>
#include <stdbool.h>

// MFT Header structure
typedef struct {
    uint32_t magic;
    uint16_t update_sequence_offset;
    uint16_t update_sequence_count;
    uint64_t logfile_sequence_number;
    uint16_t sequence_number;
    uint16_t hard_link_count;
    uint16_t attribute_offset;
    uint16_t flags;
    uint32_t used_size;
    uint32_t allocated_size;
    uint64_t file_reference;
    uint16_t next_attribute_id;
    uint32_t record_number;
    uint32_t base_record_segment;
} MFTHeader;

// MFT Attribute structure
typedef struct {
    uint32_t type;
    uint32_t length;
    uint8_t non_resident_flag;
    uint8_t name_length;
    uint16_t name_offset;
    uint16_t flags;
    uint16_t attribute_id;
    uint8_t *content;
} MFTAttribute;

// Function prototypes
MFTHeader create_mft_header(bool debug);
MFTAttribute create_mft_attribute(uint32_t attr_type, bool debug);
void destroy_mft_attribute(MFTAttribute *attr);
uint64_t generate_random_time();

#endif // MFT_STRUCTURES_H