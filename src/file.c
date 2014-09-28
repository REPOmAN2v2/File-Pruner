#define _GNU_SOURCE		// asprintf()

#include <stdio.h>		// file IO
#include <stdlib.h>		// dynamic memory
#include <string.h>		// strcmp, strrchr

#include "file.h"

#define BYTECHUNK 1024

static void file_io(File *in);
static int file_check_format(const char *filename, const char *extension);
static const char * file_find_chunk(void *haystack, const size_t length, const char* needle);
static void file_write(void *in, char *name, const size_t length, int flag);
static void file_close();

void * file_process(void *in)
{
	File *file = in;

	if (file_check_format(file->fullname, file->extension)) {
		file_io(file);
	}

	if (file->data)
		free(file->data);

	return NULL;
}

void file_io(File *in)
{
	FILE *file = fopen(in->fullname, "rb");

	const char *chunk = NULL;
	size_t total_read = 0;

	if (file == NULL) {
		fprintf(stderr, "Could not open %s\n", in->fullname);
	} else {
		fseek(file, 0, SEEK_END);
		in->len = ftell(file);
		fseek(file, 0, SEEK_SET);

		in->data = malloc(BYTECHUNK < in->len ? BYTECHUNK : in->len);

		do {
			size_t read = 0;

			read = (total_read + BYTECHUNK < in->len) ? BYTECHUNK : (in->len - total_read);
			fread(in->data, 1, read, file);

			if (chunk) {
				file_write(in->data, in->name, read, 0);
			} else {
				chunk = file_find_chunk(in->data, read, in->chunkID);
				if (chunk) {
					file_write((void *)chunk, in->name, strlen(chunk), 0);
				}
			}

			total_read += read;
		} while (total_read < in->len);

		fclose(file);
		file_close();
	}

	if (!chunk) {
		fprintf(stderr, "Chunk not found for %s\n", in->fullname);
	}
}

int file_check_format(const char *filename, const char *extension)
{
	if (!extension) // we don't want to check the extension if it's left as NULL
		return 1;

	const char *dot = strrchr(filename, '.');
	return (dot && !strcmp(dot + 1, extension));
}

const char * file_find_chunk(void *haystack, const size_t length, const char* needle)
{
	const char *data = haystack;
	const char *a, *b;

	b = needle;

    if (*b == 0) {
		return NULL;
    }

    for (int i = 0; i < length; ++data, ++i) {
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

void file_write(void *in, char *name, const size_t length, int flag)
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

	fwrite(data, 1, length, out);
}

void file_close()
{
	file_write(NULL, NULL, 0, -1);
}