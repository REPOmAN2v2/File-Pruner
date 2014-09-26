#ifndef DIRECTORY_H_
#define DIRECTORY_H_

#define dir_process(...) dir_process_fn((File){__VA_ARGS__})

struct _file;

typedef void (*level_fn)(struct _file path);

typedef struct _file {
	char *name, *fullname;
	level_fn dir_action, file_action;
	int depth, error, flag;
	unsigned long len;
	void *data;
} File;

int dir_process_fn(File level);
void dir_check_output();

#endif