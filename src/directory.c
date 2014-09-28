#define _GNU_SOURCE		// asprintf()

#include <sys/stat.h>	// struct stat and stat()
#include <stdio.h>
#include <dirent.h> 	// dirent
#include <stdlib.h> 	// free

#include "directory.h"
#include "thpool.h"		// threads
#include "gc.h"			// garbage collection

const char *output = "../output";

static void dir_process_fn(File level, void *in);
static int dir_count(DIR *in, const char *path);
static void dir_check_file(File *file, void *in);

void dir_process_wrapper(File level)
{
	thpool_t *thpool = NULL;

	if ((level.threads > 1 && level.depth == 0)) {
		fprintf(stdout, "Creating pool\n");
		thpool = thpool_init(level.threads);
	}

	dir_process_fn(level, (void *)thpool);
}

void dir_process_fn(File level, void *in)
{
	thpool_t *thpool = in;

	if (!level.fullname) {
		if (level.name) {
			level.fullname = level.name;
		} else {
			level.fullname = ".";
		}
	}

	int i = 0, filecount;

	DIR *current = opendir(level.fullname);
	if (!current) return;
	struct dirent *entry;

	if (thpool) {
		filecount = dir_count(current, level.fullname);
	} else {
		filecount = 1;
	}

	File *thread_files = NULL;
	thread_files = malloc(sizeof (File) * filecount);

	while ((entry = readdir(current))) {
		if (entry->d_name[0] == '.') continue;

		thread_files[i] = level;
		thread_files[i].name = strdup(entry->d_name);
		asprintf(&thread_files[i].fullname, "%s/%s", level.fullname, entry->d_name);

		dir_check_file(&thread_files[i], (void *)thpool);

		if (thpool)
			++i;
	}

	closedir(current);
	gc_add_garbage(thread_files);

	if (level.threads > 1 && level.depth == 0) {
		thpool_destroy(thpool, thpool_graceful);
		gc_collect_garbage();
	}
}

int dir_count(DIR *in, const char *path)
{
	struct dirent *entry;
	int count = 0;

	while ((entry = readdir(in))) {
		if (entry->d_name[0] == '.') continue;
		else ++count;
	}

	rewinddir(in);

	fprintf(stdout, "\nCounted %d files/dirs in %s\n\n", count, path);
	return count;
}

void dir_check_file(File *file, void *in)
{
	thpool_t *thpool = in;
	struct stat s;
	stat(file->fullname, &s);

	if (S_ISDIR(s.st_mode)) {
		if (file->recursive) {
			++file->depth;
			if (file->dir_action) {
				if (thpool)
					thpool_add_work(thpool, file->dir_action, (void *)file);
				else
					file->dir_action(file);
			}
			dir_process_fn(*file, thpool);
		}
	} else if (S_ISREG(s.st_mode) && file->file_action) {
		if (thpool)
			thpool_add_work(thpool, file->file_action, (void *)file);
		else
			file->file_action(file);
	}
}

void dir_check_output()
{
	struct stat st = {0};

	if (stat(output, &st) == -1) {
	    mkdir(output);
	}
}