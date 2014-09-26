#ifndef DIRECTORY_H_
#define DIRECTORY_H_

#define processDir(...) processDirFn((File){__VA_ARGS__})

struct _file;

typedef void (*level_fn)(struct _file path);

typedef struct _file {
	char *name, *fullname;
	level_fn dirAction, fileAction;
	int depth, error, flag;
	unsigned long len;
	void *data;
} File;

int processDirFn(File level);

#endif