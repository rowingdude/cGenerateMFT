#ifndef MFT_RECORD_GENERATOR_H
#define MFT_RECORD_GENERATOR_H

#include <stdbool.h>
#include "mft_structures.h"

#define RECORD_SIZE 1024

typedef struct MFTRecordGenerator MFTRecordGenerator;

MFTRecordGenerator* create_mft_record_generator(bool debug);
void destroy_mft_record_generator(MFTRecordGenerator* generator);
uint8_t* generate_record(MFTRecordGenerator* generator, bool introduce_error);

#endif // MFT_RECORD_GENERATOR_H