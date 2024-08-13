#include "mft_structures.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#define WIN32_TICKS_PER_SECOND 10000000
#define EPOCH_DIFFERENCE 11644473600LL

MFTHeader create_mft_header(bool debug) {
    MFTHeader header;
    header.magic = 0x454C4946;  // "FILE"
    header.update_sequence_offset = 42 + (rand() % 7);
    header.update_sequence_count = 1 + (rand() % 4);
    header.logfile_sequence_number = rand() % 1000001;
    header.sequence_number = 1 + (rand() % 1000);
    header.hard_link_count = 1 + (rand() % 5);
    header.attribute_offset = 56 + (rand() % 41);
    header.flags = 1 << (rand() % 4);
    header.used_size = 512 + (rand() % 513);
    header.allocated_size = 1024;
    header.file_reference = rand() % 1000001;
    header.next_attribute_id = 1 + (rand() % 100);
    header.record_number = rand() % 1000001;
    header.base_record_segment = 0;

    if (debug) {
        printf("Created MFT Header:\n");
        printf("  Magic: 0x%08X\n", header.magic);
        printf("  Update Sequence Offset: %u\n", header.update_sequence_offset);
        printf("  Update Sequence Count: %u\n", header.update_sequence_count);
        printf("  Logfile Sequence Number: %llu\n", header.logfile_sequence_number);
        printf("  Sequence Number: %u\n", header.sequence_number);
        printf("  Hard Link Count: %u\n", header.hard_link_count);
        printf("  Attribute Offset: %u\n", header.attribute_offset);
        printf("  Flags: 0x%04X\n", header.flags);
        printf("  Used Size: %u\n", header.used_size);
        printf("  Allocated Size: %u\n", header.allocated_size);
        printf("  File Reference: %llu\n", header.file_reference);
        printf("  Next Attribute ID: %u\n", header.next_attribute_id);
        printf("  Record Number: %u\n", header.record_number);
        printf("  Base Record Segment: %u\n", header.base_record_segment);
    }

    return header;
}

MFTAttribute create_mft_attribute(uint32_t attr_type, bool debug) {
    MFTAttribute attr;
    attr.type = attr_type;
    attr.non_resident_flag = 0;
    attr.name_length = 0;
    attr.name_offset = 0;
    attr.flags = 0;
    attr.attribute_id = rand() & 0xFFFF;

    size_t content_size;
    switch (attr_type) {
        case 0x10:  // Standard Information
            content_size = 13 * sizeof(uint64_t);
            attr.content = malloc(content_size);
            for (int i = 0; i < 13; i++) {
                ((uint64_t*)attr.content)[i] = (i < 4) ? generate_random_time() : rand();
            }
            break;
        case 0x30:  // File Name
            content_size = 8 * sizeof(uint64_t) + 2 * sizeof(uint8_t) + 40;  // 40 bytes for name (20 wide chars)
            attr.content = malloc(content_size);
            memset(attr.content, 0, content_size);
            for (int i = 0; i < 8; i++) {
                ((uint64_t*)attr.content)[i] = (i > 0 && i < 5) ? generate_random_time() : rand();
            }
            ((uint8_t*)attr.content)[64] = 20;  // name length
            ((uint8_t*)attr.content)[65] = rand() % 4;  // namespace
            for (int i = 0; i < 20; i++) {
                ((uint16_t*)attr.content)[33 + i] = 'a' + (rand() % 26);
            }
            break;
        default:
            content_size = 16 + (rand() % 113);
            attr.content = malloc(content_size);
            for (size_t i = 0; i < content_size; i++) {
                ((uint8_t*)attr.content)[i] = rand() & 0xFF;
            }
    }
    attr.length = content_size + 16;  // 16 bytes for the attribute header

    if (debug) {
        printf("Created MFT Attribute:\n");
        printf("  Type: 0x%08X\n", attr.type);
        printf("  Length: %u\n", attr.length);
        printf("  Non-resident Flag: %u\n", attr.non_resident_flag);
        printf("  Name Length: %u\n", attr.name_length);
        printf("  Name Offset: %u\n", attr.name_offset);
        printf("  Flags: 0x%04X\n", attr.flags);
        printf("  Attribute ID: %u\n", attr.attribute_id);
        printf("  Content Size: %zu\n", content_size);
    }

    return attr;
}

void destroy_mft_attribute(MFTAttribute *attr) {
    if (attr->content) {
        free(attr->content);
        attr->content = NULL;
    }
}

uint64_t generate_random_time() {
    time_t now = time(NULL);
    time_t random_time = now - (rand() % (20 * 365 * 24 * 60 * 60));  // Random time within last 20 years
    uint64_t windows_ticks = ((uint64_t)random_time + EPOCH_DIFFERENCE) * WIN32_TICKS_PER_SECOND;
    return windows_ticks;
}