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

static const char *input = ".";
static Flags flags;

int main(int argc, char * const argv[])
{
	if (argc == 1) {
		print_help();
	} else {
		parse_args(argc, argv);
	}

	dir_check_output();
	fprintf(stdout, "Path: %s\nOutput: %s\n\n", input, output);

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
									{"number", required_argument, 0, 'n'},
									{"string", required_argument, 0, 's'},
									{"threads", required_argument, 0, 't'},
									{"extension", required_argument, 0, 'e'},
									{0, 0, 0, 0}};

	int i = 0, c;

	while (1) {
		c = getopt_long(argc, argv, "hvrp:n:s:t:e:", long_options, &i);

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

			case 'n':
				// %i respects the number's formatting
				// i.e. 033 for octal or 0x33 for hex will be stored properly
				// instead of %d "converting" 033 to 33 (033 = 27) and
				// failing to convert 0x33
				//sscanf(optarg, "%i", &flags.chunkHex);
			break;

			case '?':
			default:
				print_help();
		}
	}

	if (!flags.chunkString) {
		fprintf(stderr, "The bytes to be identified have to be specified.\n\n");
		print_help();
	}
}

void print_help()
{
	fprintf(stdout, "--path/-p /path/to/folder\tSpecify the path to the folder containing the files to process (without trailing slash)\n"
					"--recursive/-r\t\t\tSearch subfolders.\n"
					"--extension/-e\t\t\tSpecify file extension\n"
					"--string/-s\t\t\t\tSpecify the string to be identified (required if not using -n)\n"
					"--hex/-h\t\t\t\tSpecify the string to be identified (required if not using -s)\n"
					"--threads/-t\t\t\tSpecify number of threads (defaults to 1)\n"
					"--help/-h \t\t\tPrint this help screen\n"
					"--version/-v\t\t\tPrint version information.\n");
    exit(EXIT_SUCCESS);
}

void print_version()
{
	fprintf(stdout, "File pruner 1.0\n\n"
					"The MIT License (MIT)\nCopyright (c) 2014 REPOmAN2v2\n\n");
	exit(EXIT_SUCCESS);
}