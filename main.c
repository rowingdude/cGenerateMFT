#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>
#include "mft_structures.h"
#include "mft_record_generator.h"

void print_usage(const char* program_name) {
    printf("Usage: %s [options]\n", program_name);
    printf("Options:\n");
    printf("  -o, --output FILENAME   Output filename (default: test_mft.bin)\n");
    printf("  -n, --num-records NUM   Number of records to generate (default: 100)\n");
    printf("  -e, --error-rate RATE   Error rate (0.0 to 1.0, default: 0.0)\n");
    printf("  -d, --debug             Enable debug output\n");
    printf("  -h, --help              Display this help message\n");
}

void generate_mft_file(const char* filename, int num_records, double error_rate, bool debug) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return;
    }

    MFTRecordGenerator* generator = create_mft_record_generator(debug);
    srand((unsigned int)time(NULL));

    for (int i = 0; i < num_records; i++) {
        bool introduce_error = (double)rand() / RAND_MAX < error_rate;
        uint8_t* record = generate_record(generator, introduce_error);
        fwrite(record, 1, RECORD_SIZE, file);
        free(record);
    }

    destroy_mft_record_generator(generator);
    fclose(file);
}

int main(int argc, char* argv[]) {
    const char* filename = "test_mft.bin";
    int num_records = 100;
    double error_rate = 0.0;
    bool debug = false;

    static struct option long_options[] = {
        {"output", required_argument, 0, 'o'},
        {"num-records", required_argument, 0, 'n'},
        {"error-rate", required_argument, 0, 'e'},
        {"debug", no_argument, 0, 'd'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "o:n:e:dh", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'o':
                filename = optarg;
                break;
            case 'n':
                num_records = atoi(optarg);
                if (num_records <= 0) {
                    fprintf(stderr, "Number of records must be positive\n");
                    return 1;
                }
                break;
            case 'e':
                error_rate = atof(optarg);
                if (error_rate < 0.0 || error_rate > 1.0) {
                    fprintf(stderr, "Error rate must be between 0.0 and 1.0\n");
                    return 1;
                }
                break;
            case 'd':
                debug = true;
                break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    generate_mft_file(filename, num_records, error_rate, debug);
    printf("Generated %s with %d records and %.1f%% error rate.\n", filename, num_records, error_rate * 100);

    return 0;
}