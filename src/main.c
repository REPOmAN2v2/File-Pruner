#include <stdio.h>		// IO
#include <stdlib.h>		// dynamic mem and exit()
#include <stdbool.h>	// boolean status flags
#include <string.h>		// strcmp
#include <getopt.h>		// getopt_long
#include <time.h>

#include "file.h"		// file_process
#include "directory.h"	// dir_process, dir_check_output

static void parse_args(const unsigned int argc, char * const argv[]);
static void print_help();
static void print_version();

static const char *extension = NULL;

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

	dir_process(.name = strdup(input), .file_action = file_process,
				.recursive = flags.recursive, .threads = flags.threads,
				.extension = extension);

	t = clock() - t;
	double time_taken = ((double)t)/CLOCKS_PER_SEC;
	printf("%f seconds \n", time_taken);
}

void parse_args(const unsigned int argc, char * const argv[])
{
	bool eFlag = false;

	struct option long_options[] = {{"help", no_argument, 0, 'h'},
									{"version", no_argument, 0, 'v'},
									{"recursive", no_argument, 0, 'r'},
									{"path", required_argument, 0, 'p'},
									{"threads", required_argument, 0, 't'},
									{"extension", required_argument, 0, 'e'},
									{0, 0, 0, 0}};

	int i = 0, c;

	while (1) {
		c = getopt_long(argc, argv, "hvrp:t:e:", long_options, &i);

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

			case 't':
				flags.threads = atoi(optarg);
			break;

			case 'e':
				extension = optarg;
				eFlag = true;
			break;

			case '?':
			default:
				print_help();
		}
	}

	if (!eFlag) {
		fprintf(stderr, "The file extension has to be specified.\n\n");
		print_help();
	}
}

void print_help()
{
	fprintf(stdout, "--path/-p /path/to/folder\tSpecify the path to the folder containing the files to process (without trailing slash)\n"
					"--recursive/-r\t\t\tSearch subfolders.\n"
					"--extension/-e\t\t\tSpecify file extension (required)\n"
					"--threads/-t\t\t\tSpecify number of threads (defaults to 1)\n"
					"--help/-h \t\t\tPrint this help screen\n"
					"--version/-v\t\t\tPrint version information.\n");
    exit(EXIT_SUCCESS);
}

void print_version()
{
	fprintf(stdout, "File pruner 0.9\n\n"
					"The MIT License (MIT)\nCopyright (c) 2014 REPOmAN2v2\n\n");
	exit(EXIT_SUCCESS);
}