#define _GNU_SOURCE		// asprintf()

#include <stdio.h>		// file IO
#include <stdlib.h>		// dynamic memory
#include <string.h>		// strcmp, strrchr
#include "include.h"	// global variables
#include "file.h"

#define BYTECHUNK 1024

static const unsigned int chunkID = 0x52494646;

static long file_read(File *in);
static int file_check_format(const char *filename);
static long file_find_chunk(void *in, int chunk);
static void file_write(File in, const long chunk, int flag);
static void file_close();

void file_process(File in)
{
	int errcnt = 0;
	//fprintf(stdout, "Processing %s\n", in.fullname);

	if (file_check_format(in.fullname)) {
		long offset = file_read(&in);
		fprintf(stdout, "Offset: %ld\n", offset);

		if (offset == -1) {
			fprintf(stderr, "Chunk not found for %s\n", in.fullname);
			++errcnt;
		} /*else {
			file_write(in, offset);
			//file_write_header(in, offset);
		}*/
	}

	if (in.data)
		free(in.data);

	in.error = errcnt;
}

long file_read(File *in)
{
	FILE *file = fopen(in->fullname, "rb");
	/*char *path;
	asprintf(&path, "%s/header-%s", output, in->name);
	FILE *out = fopen(path, "wb");*/

	int found_chunk = 0;
	long chunk_pos = 0;
	unsigned long total_read = 0;

	if (file == NULL) {
		fprintf(stderr, "Could not open %s\n", in->fullname);
	} else {
		//fprintf(stdout, "Opened file with success\n");

		fseek(file, 0, SEEK_END);
		in->len = ftell(file);
		fseek(file, 0, SEEK_SET);

		in->data = malloc(BYTECHUNK);
		//fprintf(stderr, "Allocated %lu bytes\n", in->len);

		do {
			unsigned long read = 0;

			read = (total_read + BYTECHUNK < in->len) ? BYTECHUNK : (in->len - total_read);
			size_t result = fread(in->data, 1, read, file);

			if (found_chunk) {
				//fwrite(in->data, 1, read, out);
				file_write(*in, read - chunk_pos, 0);
			} else {
				chunk_pos = file_find_chunk(in->data, read);
				if (chunk_pos > 0) {
					found_chunk = 1;
					//fwrite(in->data, 1, read - chunk_pos, out);
					file_write(*in, read - chunk_pos, 0);
					chunk_pos += total_read;
				}
			}

			total_read += read;
			//fprintf(stdout, "found_chunk: %d, total_read: %ld, read: %ld\n", found_chunk, total_read, read);

			/*if (result != read) {
				if (ferror(file)) {
					perror("fread()");
					fprintf(stderr, "fread() failed\n");
	            } else {
	            	fprintf(stderr, "Reading error\n");
	            }
			} else {
				if (found_chunk) {
					file_write(*in, NULL, read, 0);
				} else {
					chunk_pos = file_find_chunk(in->data, read);
					if (chunk_pos > 0) {
						found_chunk = 1;
						chunk_pos += total_read;
						file_write(*in, NULL, read - chunk_pos, 0);
					}
				}
			}
			total_read += read;*/

		} while (total_read < in->len);

		fclose(file);
		file_close();
	}

	return chunk_pos;
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
		//fprintf(stdout, "Code: 0x%X\n", code);
		if (code == chunkID) {
			//fprintf(stdout, "Found it at byte 0x%X\n", data[pos]);
			return pos;
		} else {
			pos += 4;
		}
	}

	//fprintf(stderr, "Chunk not found for %s\n", in.fullname);
	return -1;
}

void file_write(File in, const long chunk, int flag)
{
	static FILE *out;

	//fprintf(stdout, "Writing, %p\n", out);

	if (flag == -1) {
		if (out) {
			fclose(out);
			out = NULL;
		}
		return;
	}

	/*if (!name)
		const char *name = "";*/



	if (!out) {
		asprintf(&in.fullname, "%s/%s", output, in.name);
		out = fopen(in.fullname, "wb");
	}

	long err = fwrite(in.data, 1, chunk, out);

	//fprintf(stdout, "Wrote %ld bytes\n", err);
}

void file_close()
{
	file_write((File){0}, 0, -1);
}

/*void file_write(File in, const long offset)
{
	unsigned long total = 0;

	FILE *input = fopen(in.fullname, "rb");
	asprintf(&in.fullname, "%s/header-%s", output, in.name);
	FILE *header = fopen(in.fullname, "wb");

	while (total < offset) {
		unsigned long read = 0;

		read = (total + BYTECHUNK < offset) ? BYTECHUNK : (offset - total);
		size_t result = fread(in.data, 1, read, input);
		total += read;

		if (result != read) {
			if (ferror(input)) {
				perror("fread()");
				fprintf(stderr, "fread() failed\n");
            } else {
            	fprintf(stderr, "Reading error\n");
            }
		} else {
			fwrite(in.data, 1, read, header);
		}
	}

	fclose(header);
	fprintf(stdout, "Wrote %ld bytes, offset = %ld\n", total, offset);

	asprintf(&in.fullname, "%s/%s", output, in.name);
	FILE *output = fopen(in.fullname, "wb");

	while (total < in.len) {
		unsigned long read = 0;

		read = (total + BYTECHUNK < in.len) ? BYTECHUNK : (in.len - total);
		size_t result = fread(in.data, 1, read, input);

		if (result != read) {
			if (ferror(input)) {
				perror("fread()");
				fprintf(stderr, "fread() failed\n");
            } else {
            	//fprintf(stderr, "Reading error\n");
            }
		} else {
			fwrite(in.data, 1, read, output);
		}
	}

	fclose(output);
	fclose(input);
}

void file_write_header(File in, const long offset)
{


	//fwrite(in.data, 1, offset, file);

	//fclose(file);
	free(in.fullname);
}*/