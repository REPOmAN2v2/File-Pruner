#define _GNU_SOURCE		// asprintf()

#include <stdio.h>		// file IO
#include <stdlib.h>		// dynamic memory
#include <string.h>		// strcmp, strrchr
#include "include.h"	// global variables
#include "file.h"

#define BYTECHUNK 1024

static const unsigned int chunkID = 0x52494646;

static void file_io(File *in);
static int file_check_format(const char *filename);
static long file_find_chunk(void *in, int chunk);
static void file_write(void *in, char *name, const long chunk, int flag);
static void file_close();

void file_process(File in)
{
	if (file_check_format(in.fullname)) {
		file_io(&in);
	}

	if (in.data)
		free(in.data);
}

void file_io(File *in)
{
	FILE *file = fopen(in->fullname, "rb");

	int found_chunk = 0;
	long chunk_pos = 0;
	unsigned long total_read = 0;

	if (file == NULL) {
		fprintf(stderr, "Could not open %s\n", in->fullname);
	} else {
		fseek(file, 0, SEEK_END);
		in->len = ftell(file);
		fseek(file, 0, SEEK_SET);

		in->data = malloc(BYTECHUNK);

		do {
			unsigned long read = 0;

			read = (total_read + BYTECHUNK < in->len) ? BYTECHUNK : (in->len - total_read);
			fread(in->data, 1, read, file);

			if (found_chunk) {
				file_write(in->data, in->name, read, 0);
			} else {
				chunk_pos = file_find_chunk(in->data, read);
				if (chunk_pos > 0) {
					found_chunk = 1;
					file_write(in->data + chunk_pos, in->name, read - chunk_pos, 0);
					chunk_pos += total_read;
				}
			}

			total_read += read;
		} while (total_read < in->len);

		fclose(file);
		file_close();
	}

	if (chunk_pos == -1) {
		fprintf(stderr, "Chunk not found for %s\n", in->fullname);
	}
}

int file_check_format(const char *filename)
{
	const char *dot = strrchr(filename, '.');
	return (dot && !strcmp(dot + 1, extension));
}

long file_find_chunk(void *in, int chunk)
{
	long pos = 0;
	unsigned int code;
	char *data = in;

	while (pos < chunk) {
		code = data[pos] << 24 | data[pos+1] << 16 | data[pos+2] << 8
			   | data[pos+3];

		if (code == chunkID) {
			fprintf(stdout, "Found 0x%X at byte %d\n", code, data[pos]);
			return pos;
		} else {
			pos += 4;
		}
	}

	return -1;
}

void file_write(void *in, char *name, const long chunk, int flag)
{
	static _Thread_local FILE *out;
	const char *data = in;

	if (flag == -1) {
		if (out) {
			fclose(out);
			out = NULL;
		}
		return;
	}

	if (!out) {
		asprintf(&name, "%s/%s", output, name);
		//fprintf(stdout, "Writing %s\n", name);
		out = fopen(name, "wb");
	}

	fwrite(data, 1, chunk, out);
}

void file_close()
{
	file_write(NULL, NULL, 0, -1);
}