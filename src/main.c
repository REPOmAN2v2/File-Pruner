#define _GNU_SOURCE		// asprintf()

#include <stdio.h>		// IO
#include <stdlib.h>		// dynamic mem and exit()
//#include <stdbool.h>	// boolean status flags
#include <string.h>		// strcmp
#include <getopt.h>		// getopt_long
#include <errno.h>
#include <time.h>

#include "file.h"		// file_process
#include "directory.h"	// dir_process, dir_check_output

static void parse_args(const unsigned int argc, char * const argv[]);
static void print_help();
static void print_version();
static unsigned char * hex_to_byte(char *hex);

static const char *input = ".";
static Flags flags = {.output = "./output"};

int main(int argc, char * const argv[])
{
	if (argc == 1) {
		print_help();
	} else {
		parse_args(argc, argv);
	}

	dir_check_output(flags.output);
	fprintf(stdout, "Path: %s\nOutput: %s\n\n", input, flags.output);

	clock_t t = clock();

	dir_process(.name = strdup(input), .file_action = file_process, .flags = flags);

	t = clock() - t;
	double time_taken = ((double)t)/CLOCKS_PER_SEC;
	printf("%f seconds \n", time_taken);
}

void parse_args(const unsigned int argc, char * const argv[])
{
	struct option long_options[] = {{"help", no_argument, 0, 'h'},
									{"version", no_argument, 0, 'v'},
									{"recursive", no_argument, 0, 'r'},
									{"path", required_argument, 0, 'p'},
									{"hex", required_argument, 0, 'n'},
									{"string", required_argument, 0, 's'},
									{"output", required_argument, 0, 'o'},
									{"threads", required_argument, 0, 't'},
									{"extension", required_argument, 0, 'e'},
									{0, 0, 0, 0}};

	int i = 0, c;

	while (1) {
		c = getopt_long(argc, argv, "hvrp:n:s:o:t:e:", long_options, &i);

		if (c == -1) break;

		switch (c) {
			case 'h':
				print_help();
			break;

			case 'v':
				print_version();
			break;

			case 'r':
				flags.recursive = 1;
			break;

			case 'p':
				input = optarg;
			break;

			case 't': {
				char *endptr = NULL;
				flags.threads = strtol(optarg, &endptr, 0);
				if (errno == ERANGE || *endptr != '\0') {
					fprintf(stderr, "Invalid or out of range number of threads.\n");
					exit(EXIT_FAILURE);
				}
			}
			break;

			case 'e':
				flags.extension = optarg;
			break;

			case 's':
				flags.chunkString = optarg;
			break;

			case 'o':
				flags.output = optarg;
			break;

			case 'n':
				flags.chunkString = hex_to_byte(optarg);
			break;

			case '?':
			default:
				print_help();
		}
	}

	if (!flags.chunkString) {
		fprintf(stderr, "The sequence to be identified has to be specified.\n\n");
		print_help();
	}
}

void print_help()
{
	fprintf(stdout, "--path/-p /path/to/folder\tSpecify the input folder (no trailing slash) - Defaults to '.'\n"
					"--ouput/-o /path/to/folder\tSpecify the output folder (no trailing slash) - Defaults to './output'\n"
					"--recursive/-r\t\t\tSearch subfolders. - Defaults to no\n"
					"--extension/-e\t\t\tSpecify file extension - Defaults to null\n"
					"--string/-s\t\t\tSpecify the string to be identified (required if not using -n)\n"
					"--hex/-n\t\t\tSpecify the hex sequence to be identified without leading 0x (required if not using -s)\n"
					"--threads/-t\t\t\tSpecify number of threads - Defaults to 1)\n"
					"--help/-h \t\t\tPrint this help screen\n"
					"--version/-v\t\t\tPrint version information.\n");
    exit(EXIT_SUCCESS);
}

void print_version()
{
	fprintf(stdout, "File pruner 1.2\n\n"
					"The MIT License (MIT)\nCopyright (c) 2014 REPOmAN2v2\n\n");
	exit(EXIT_SUCCESS);
}

/*
	Using sscanf(hex, "%x", bytes) with hex and bytes arrays of chars is
	somewhat undefined. On little-endian systems, unaligned memory accesses are
	allowed but the result is a reversed byte string. On big-endian systems,
	unaligned memory accesses are not allowed. Hence using the "%-x" modifier
	to modify the endianness when reading with sscanf does not work.

	This solution is more portable and defined.
*/
unsigned char * hex_to_byte(char *hex)
{
	size_t len = strlen(hex)/2;
	unsigned char *bytes = NULL;

	if (len % 2) {
		asprintf(&hex, "0%s", hex);
		++len;
	}

	bytes = malloc(len+1);
	bytes[len] = '\0';

	for (size_t i = 0; i < len; ++i) {
		// read an hex number of length 2 to be stored in a (unsigned) char
		sscanf(hex + 2*i, "%2hhx", &bytes[i]);
	}

	free(hex);
	return bytes;
}