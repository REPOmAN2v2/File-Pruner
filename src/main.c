#include <sys/stat.h>	// struct stat and stat()
#include <stdio.h>		// IO
#include <stdlib.h>		// dynamic mem and exit()
#include <stdbool.h>	// boolean status flags

#include "directory.h"	// directory functionality
#include "file.h"		// file processing

static void checkOutputDir();
static void parseArgs(const unsigned int argc, const char * const argv[]);
static void printHelp();
static void printVersion();

const unsigned int chunkID = 0x52494646;
const char *extension = NULL;
const char *output = "../output";

static const char *input = NULL;
static bool recursive = false;

int main(int argc, char * const argv[])
{
	if (argc == 1) {
		printHelp();
	} else {
		parseArgs(argc, (const char * const *)argv);
	}

	checkOutputDir();
	fprintf(stdout, "Path: %s\nOutput: %s\n\n", input, output);

	processDir(.name = strdup(input), .fileAction = processFile, .flag = recursive);
}

void checkOutputDir()
{
	struct stat st = {0};

	if (stat(output, &st) == -1) {
	    mkdir(output);
	}
}

void parseArgs(const unsigned int argc, const char * const argv[])
{
	bool eFlag = false;
	bool pFlag = false;

	for (size_t i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			printHelp();
		} else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
			printVersion();
		} else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--path") == 0) {
			if (i + 1 <= argc - 1) {
				++i;
				input = argv[i];
				pFlag = true;
			}
		} else if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--extension") == 0) {
			if (i + 1 <= argc - 1) {
				++i;
				extension = argv[i];
				eFlag = true;
			}
		} else if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--recursive") == 0) {
			recursive = true;
		}
	}

	if (!pFlag) input = ".";

	if (!eFlag) {
		fprintf(stderr, "The file extension has to be specified.\n\n");
		printHelp();
	}
}

void printHelp()
{
	fprintf(stdout, "--path/-p /path/to/folder\tSpecify the path to the folder containing the files to process (without trailing slash)\n"
					"--recursive/-r\t\t\tSearch subfolders.\n"
					"--extension/-e\t\t\tSpecify file extension (required)\n"
					"--help/-h \t\t\tPrint this help screen\n"
					"--version/-v\t\t\tPrint version information.\n");
    exit(EXIT_SUCCESS);
}

void printVersion()
{
	fprintf(stdout, "File pruner 0.5\n\n"
					"The MIT License (MIT)\nCopyright (c) 2014 REPOmAN2v2\n\n");
	exit(EXIT_SUCCESS);
}