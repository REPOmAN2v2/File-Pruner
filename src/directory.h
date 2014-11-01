#ifndef DIRECTORY_H_
#define DIRECTORY_H_

#include <stdio.h>	// FILE

#define dir_process(...) dir_process_wrapper((File){__VA_ARGS__})

typedef struct _flags {
	int recursive;
	int threads;
	//const int chunkHex;
	const char *extension;
	const unsigned char *chunkString;
} Flags;

extern const char *output;

struct _file;

typedef void *(*level_fn)(void *path);

typedef struct _file {
	char *name, *fullname;
	level_fn dir_action, file_action;
	FILE *fp;
	int depth;
	union {
		struct _flags;
		Flags flags;
	};
	unsigned long len;
	void *data;
} File;

void dir_process_wrapper(File level);
void dir_check_output();

#endif