#include "mft_record_generator.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct MFTRecordGenerator {
    bool debug;
    uint32_t attribute_types[16];
};

MFTRecordGenerator* create_mft_record_generator(bool debug) {
    MFTRecordGenerator* generator = malloc(sizeof(MFTRecordGenerator));
    generator->debug = debug;
    
    // Initialize attribute types
    for (int i = 0; i < 16; i++) {
        generator->attribute_types[i] = 0x10 + (i * 0x10);
    }
    
    if (debug) {
        printf("Created MFT Record Generator\n");
    }
    
    return generator;
}

void destroy_mft_record_generator(MFTRecordGenerator* generator) {
    if (generator->debug) {
        printf("Destroying MFT Record Generator\n");
    }
    free(generator);
}

static void introduce_error(uint8_t* record, bool debug) {
    int error_type = rand() % 3;
    if (debug) {
        printf("Introducing error type: %d\n", error_type);
    }
    switch (error_type) {
        case 0: // corrupt_magic
            memset(record, 0, 4);
            if (debug) printf("Corrupted magic number\n");
            break;
        case 1: // invalid_size
            *((uint32_t*)(record + 24)) = 2048;
            if (debug) printf("Set invalid size: 2048\n");
            break;
        case 2: // corrupt_attribute
            {
                int attr_start = 64 + (rand() % (RECORD_SIZE - 88));
                record[attr_start] = 0xFF;
                record[attr_start + 1] = 0xFF;
                record[attr_start + 2] = 0x00;
                record[attr_start + 3] = 0x00;
                if (debug) printf("Corrupted attribute at offset %d\n", attr_start);
            }
            break;
    }
}

uint8_t* generate_record(MFTRecordGenerator* generator, bool introduce_error_flag) {
    uint8_t* record = calloc(RECORD_SIZE, 1);
    
    // Generate MFT Header
    MFTHeader header = create_mft_header(generator->debug);
    memcpy(record, &header, sizeof(MFTHeader));
    
    size_t offset = sizeof(MFTHeader);
    
    if (generator->debug) {
        printf("Generating record:\n");
        printf("  Header size: %zu\n", sizeof(MFTHeader));
    }
    
    // Add attributes
    int num_attributes = 1 + (rand() % 5);
    for (int i = 0; i < num_attributes; i++) {
        uint32_t attr_type = generator->attribute_types[rand() % 16];
        MFTAttribute attr = create_mft_attribute(attr_type, generator->debug);
        
        if (offset + attr.length <= RECORD_SIZE - 8) {
            memcpy(record + offset, &attr, 16);  // Copy attribute header
            memcpy(record + offset + 16, attr.content, attr.length - 16);  // Copy attribute content
            offset += attr.length;
            
            if (generator->debug) {
                printf("  Added attribute type 0x%x, size: %u\n", attr_type, attr.length);
            }
        } else if (generator->debug) {
            printf("  Skipped attribute type 0x%x due to size constraints\n", attr_type);
        }
        
        destroy_mft_attribute(&attr);
    }
    
    // Add end marker
    *((uint32_t*)(record + offset)) = 0xFFFFFFFF;
    
    if (introduce_error_flag) {
        introduce_error(record, generator->debug);
    }
    
    if (generator->debug) {
        printf("Generated record size: %zu\n", offset);
    }
    
    return record;
}