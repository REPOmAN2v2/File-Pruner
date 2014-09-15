#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <stdbool.h>

bool checkFormat(const char *filename);
int listDir(const char *path);
int readFile(const char *path, const char *filename);
FILE * openFile(const char *path, const char *filename, const char *flag);
void editFile(char *buffer, unsigned long len, const char *filename);
void writeNew(char *buffer, const int offset, unsigned long len, const char *filename);

const unsigned int chunkID = 0x52494646;
const char *output = "./output";

int main(int argc, char * const argv[])
{
	struct stat st = {0};

	if (stat(output, &st) == -1) {
	    mkdir(output);
	}

	fprintf(stdout, "Path: %s\nOutput: %s\n\n", argv[1], output);

	listDir(argv[1]);
}

int listDir(const char *path) 
{
    struct dirent *entry;
    DIR *dp;

    dp = opendir(path);
    if (dp == NULL) {
        perror("opendir: Path does not exist or could not be read.");
        return -1;
    }

    while ((entry = readdir(dp)))
        if (checkFormat(entry->d_name)) {
        	readFile(path, entry->d_name);
        }

    closedir(dp);
    return 0;
}

bool checkFormat(const char *filename)
{
	size_t len = strlen(filename);
	if (filename[len-4] == '.'
		&& filename[len-3] == 'b'
		&& filename[len-2] == 'i'
		&& filename[len-1] == 'g') {
		puts(filename);
		return true;
	}

	return false;
}

int readFile(const char *path, const char *filename)
{
	FILE *file = openFile(path, filename, "rb");
	
	if (file == NULL) {
		fprintf(stderr, "Could not open %s%s\n", path, filename);
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

void editFile(char *buffer, unsigned long len, const char *filename)
{
	// 0x52494646
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

void writeNew(char *buffer, const int offset, unsigned long len, const char *filename)
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
}