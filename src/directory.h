#ifndef DIRECTORY_H_
#define DIRECTORY_H_

#define dir_process(...) dir_process_wrapper((File){__VA_ARGS__})

typedef struct _flags {
	int recursive;
	int threads;
} Flags;

extern const char *output;

struct _file;

typedef void *(*level_fn)(void *path);

typedef struct _file {
	char *name, *fullname;
	const char *extension;
	level_fn dir_action, file_action;
	int depth;
	struct _flags;
	unsigned long len;
	void *data;
} File;

void dir_process_wrapper(File level);
void dir_check_output();

#endif