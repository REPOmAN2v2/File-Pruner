#ifndef DIRECTORY_H_
#define DIRECTORY_H_

#define dir_process(...) dir_process_wrapper((File){__VA_ARGS__})

typedef struct _flags {
	int recursive;
	int threads;
	const char *extension;
	const char *chunkID;
} Flags;

extern const char *output;

struct _file;

typedef void *(*level_fn)(void *path);

typedef struct _file {
	char *name, *fullname;
	level_fn dir_action, file_action;
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