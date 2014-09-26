#define _GNU_SOURCE		// asprintf()

#include <stdio.h>		// file IO
#include <stdlib.h>		// dynamic memory
#include <string.h>		// strcmp, strrchr
#include "include.h"	// global variables
#include "file.h"

static const unsigned int chunkID = 0x52494646;

static int file_read(File *in);
static int file_check_format(const char *filename);
static long file_find_chunk(File in);
static void file_write(File in, const long offset);
static void file_write_header(File in, const long offset);

void file_process(File in)
{
	int errcnt = 0;
	fprintf(stdout, "Processing %s\n", in.fullname);

	if (file_check_format(in.fullname)) {
		if (!file_read(&in)) {
			long offset = file_find_chunk(in);

			if (offset == -1)
				++errcnt;
			else {
				file_write(in, offset);
				file_write_header(in, offset);
			}
		} else ++errcnt;
	}

	if (in.data)
		free(in.data);

	in.error = errcnt;
}

int file_read(File *in)
{
	FILE *file = fopen(in->fullname, "rb");

	if (file == NULL) {
		fprintf(stderr, "Could not open %s\n", in->fullname);
	} else {
		fprintf(stdout, "Opened file with success\n");

		fseek(file, 0, SEEK_END);
		in->len = ftell(file);
		fseek(file, 0, SEEK_SET);

		in->data = malloc(in->len * sizeof(char));
		fprintf(stderr, "Allocated %lu bytes\n", in->len);

		size_t result = fread(in->data, sizeof(char), in->len, file);
		fclose(file);

		if (result != in->len) {
			if (ferror(file)) {
				perror("fread()");
				fprintf(stderr, "fread() failed\n");
            } else {
            	fprintf(stderr, "Reading error\n");
            }
            in->error = 1;
		}
	}

	return in->error;
}

int file_check_format(const char *filename)
{
	const char *dot = strrchr(filename, '.');
	return (dot && !strcmp(dot + 1, extension));
}

long file_find_chunk(File in)
{
	long pos = 0;
	unsigned int code;
	char *data = in.data;

	while (pos < in.len) {
		code = data[pos] << 24 | data[pos+1] << 16 | data[pos+2] << 8
			   | data[pos+3];
		//fprintf(stdout, "Code: 0x%X\n", code);
		if (code == chunkID) {
			fprintf(stdout, "Found it at byte 0x%X\n", data[pos]);
			return pos;
		} else {
			pos += 4;
		}
	}

	fprintf(stderr, "Code not found, unrecognised filetype for %s\n", in.fullname);
	return -1;
}

void file_write(File in, const long offset)
{
	asprintf(&in.fullname, "%s/%s", output, in.name);
	FILE *file = fopen(in.fullname, "wb");

	in.len -= offset;
	fprintf(stdout, "Writing %lu bytes\n\n", in.len);
	fwrite(in.data + offset, 1 , in.len, file);

	fclose(file);
	free(in.fullname);
}

void file_write_header(File in, const long offset)
{
	asprintf(&in.fullname, "%s/header-%s", output, in.name);
	FILE *file = fopen(in.fullname, "wb");

	fwrite(in.data, 1, offset, file);

	fclose(file);
	free(in.fullname);
}