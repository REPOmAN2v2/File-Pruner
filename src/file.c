#define _GNU_SOURCE		// asprintf()

#include <stdio.h>		// file IO
#include <stdlib.h>		// dynamic memory
#include <string.h>		// strcmp, strrchr
#include <stdint.h>		// C99 types
#include <sys/stat.h>	// fstat, struct stat

#include "file.h"

#define BYTECHUNK 4096

static void file_read(File *in);
static size_t file_remap(File *in, size_t total);
static void file_write(File *in, const unsigned char *offset, size_t read);
static int file_check_format(const char *filename, const char *extension);
static void file_get_length(File *in);
static uint8_t * file_find_chunk(void *haystack, const size_t length, const unsigned char* needle);
static size_t get_alloc_size(size_t a) {return BYTECHUNK < a ? BYTECHUNK : a;};


void * file_process(void *in)
{
	File *file = in;
	fprintf(stdout, "Processing file %s, looking for %s\n", file->fullname, file->chunkString);

	if (file_check_format(file->fullname, file->extension)) {
		file_read(file);
	}

	if (file->data)
		free(file->data);

	return NULL;
}

void file_read(File *in)
{
	uint8_t *offset = NULL;
	size_t total_read = 0, read = 0;

	in->fp = fopen(in->fullname, "rb");

	if (!in->fp) {
		fprintf(stderr, "Could not open %s\n", in->fullname);
		return;
	}

	file_get_length(in);

	do {
		read = file_remap(in, total_read);
		offset = file_find_chunk(in->data, read, in->chunkString/*, sizeof(in->chunkString)*/);
		total_read += read;
	} while ((total_read < in->len) && !offset);

	if (offset) {
		fprintf(stdout, "Chunk for file (%s) found\n", in->fullname);
		file_write(in, offset, read);
	} else {
		fprintf(stdout, "File (%s) was not trimmed\n", in->fullname);
	}

	fclose(in->fp);
}

void file_get_length(File *in)
{
	fseek(in->fp, 0, SEEK_END);
	in->len = ftell(in->fp);
	fseek(in->fp, 0, SEEK_SET);
}

int file_check_format(const char *filename, const char *extension)
{
	if (!extension) // we don't want to check the extension if it's left as NULL
		return 1;

	const char *dot = strrchr(filename, '.');
	return (dot && !strcmp(dot + 1, extension));
}

uint8_t * file_find_chunk(void *haystack, const size_t length, const unsigned char* needle)
{
	uint8_t *data = haystack;
	const unsigned char *a, *b;

	b = needle;

    if (*b == 0) {
		return NULL;
    }

    for (size_t i = 0; i < length; ++data, ++i) {
		if (*data != *b) {
		    continue;
		}

		a = data;

		while (1) {
		    if (*b == 0) {
				return data;
		    }
		    if (*a++ != *b++) {
				break;
		    }
		}

		b = needle;
    }

    return NULL;
}

void file_write(File *in, const unsigned char *offset, size_t read)
{
	unsigned char *data = in->data;
	File out = (File){.name = in->name,
				.len = data + (in->len - 1) - offset};
	size_t total_read = data + read - offset;

	asprintf(&out.fullname, "%s/%s", in->output, out.name);
	out.fp = fopen(out.fullname, "wb");

	if (!out.fp) {
		fprintf(stderr, "file_open(%s) failed: %d\n", out.fullname, errno);
		perror("Error");
		free(out.fullname);
		return;
	}

	fwrite(offset, 1, total_read, out.fp);

	do {
		read = file_remap(in, total_read);
		fwrite(in->data, 1, read, out.fp);
		total_read += read;
	} while((total_read < out.len));

	fclose(out.fp);
	free(out.fullname);
}

size_t file_remap(File *in, size_t total)
{
	if (!in->data) {
		in->data = malloc(get_alloc_size(in->len));
	}

	size_t read = (total + BYTECHUNK < in->len) ? BYTECHUNK : (in->len - total);
	fread(in->data, 1, read, in->fp);

	return read;
}