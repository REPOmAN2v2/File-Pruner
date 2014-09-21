#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <stdbool.h>

static bool checkFormat(const char *filename);
static int parseDir();
static void readFile(const char *filename);
static FILE * openFile(const char *path, const char *filename, const char *flag);
static void editFile(const char *buffer, unsigned long len, const char *filename);
static void writeNew(const char *buffer, const int offset, unsigned long len, const char *filename);
static void parseArgs(const unsigned int argc, const char * const argv[]);
static void printHelp();
static void printVersion();

static const unsigned int chunkID = 0x52494646;
static const char *input = NULL;
static const char *extension = NULL;
static const char *output = "./output";

int main(int argc, char * const argv[])
{
	if (argc == 1) {
		printHelp();
	} else {
		parseArgs(argc, (const char * const *)argv);
	}

	struct stat st = {0};

	if (stat(output, &st) == -1) {
	    mkdir(output);
	}

	fprintf(stdout, "Path: %s\nOutput: %s\n\n", input, output);

	parseDir();
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
		}
	}

	if (!pFlag || !eFlag) {
		fprintf(stderr, "The path and file extension have to be specified.\n\n");
		printHelp();
	}
}

int parseDir() 
{
    struct dirent *entry;
    DIR *dp;

    dp = opendir(input);
    if (dp == NULL) {
        perror("opendir: Path does not exist or could not be read.");
        return -1;
    }

    while ((entry = readdir(dp)))
        if (checkFormat(entry->d_name)) {
        	readFile(entry->d_name);
        }

    closedir(dp);
    return 0;
}

bool checkFormat(const char *filename)
{
	const char *dot = strrchr(filename, '.');

	if (dot && !strcmp(dot + 1, extension)) {
		return true;
	} else {
		return false;
	}
}

void readFile(const char *filename)
{
	FILE *file = openFile(input, filename, "rb");
	
	if (file == NULL) {
		fprintf(stderr, "Could not open %s%s\n", input, filename);
	} else {
		fprintf(stdout, "Opened file with success\n");
		fseek(file, 0, SEEK_END);
		unsigned long fileLen = ftell(file);
		fseek(file, 0, SEEK_SET);

		char *buffer = NULL;
		buffer = malloc(fileLen	* sizeof(char));
		fprintf(stderr, "Allocated %lu bytes\n", fileLen);

		size_t result = fread(buffer, sizeof(char), fileLen, file);
		fclose(file);

		if (result != fileLen) {
			if (ferror(file)) {
				perror("fread()");
				fprintf(stderr, "fread() failed\n");
            } else {
            	fprintf(stderr, "Reading error\n");
            }
		} else {
			editFile(buffer, fileLen, filename);
		}

		free(buffer);
	}
}

FILE * openFile(const char *path, const char *filename, const char *flag)
{
	FILE *file = NULL;
	char *fullpath = NULL;

	asprintf(&fullpath, "%s%s", path, filename);
	file = fopen(fullpath, flag);
	free(fullpath);

	return file;
}

void editFile(const char *buffer, unsigned long len, const char *filename)
{
	unsigned int pos = 0, code;
	while ( pos < len) {
		code = buffer[pos] << 24 | buffer[pos+1] << 16 | buffer[pos+2] << 8 | buffer[pos+3];
		//fprintf(stdout, "Code: 0x%X\n", code);
		if (code == chunkID) {
			fprintf(stdout, "Found it at byte 0x%X\n", buffer[pos]);
			writeNew(buffer, pos, len, filename);
			return;
		} else {
			pos += 4;
		}
	}

	fprintf(stderr, "Code not found, unrecognised filetype for %s\n", filename);
}

void writeNew(const char *buffer, const int offset, unsigned long len, const char *filename)
{
	char *filepath = NULL;
	asprintf(&filepath, "%s/", output);

	FILE *file = openFile(filepath, filename, "wb");
	len -= offset;
	fprintf(stdout, "Writing %lu bytes\n\n", len);
	fwrite(buffer + offset, 1 , len, file);
	fclose(file);

	asprintf(&filepath, "%sheader-", filepath);
	file = openFile(filepath, filename, "wb");
	fwrite(buffer, 1, offset, file);
	fclose(file);

	free(filepath);
}

void printHelp()
{
	fprintf(stdout, "--path/-p /path/to/folder/\tSpecify the path to the folder containing the files to process (with trailing slash)\n"
					"--help/-h \t\t\tPrint this help screen\n"
					"--version/-v\t\t\tPrint version information.\n");
    exit(EXIT_SUCCESS);
}

void printVersion()
{
	fprintf(stdout, "File pruner 0.3\n\n"
					"The MIT License (MIT)\nCopyright (c) 2014 REPOmAN2v2\n\n");
	exit(EXIT_SUCCESS);
}