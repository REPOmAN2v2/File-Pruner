#define _GNU_SOURCE		// asprintf()

#include "directory.h"
#include "include.h"	// global constants
#include <sys/stat.h>	// struct stat and stat()
#include <stdio.h>
#include <dirent.h> 	// dirent
#include <stdlib.h> 	// free
#include "thpool.h"		// threads

const char *output = "../output";

static int dir_count(DIR *in, const char *path);
static void * check_file(void *in);

int dir_process_fn(File level)
{
	if (!level.fullname) {
		if (level.name) {
			level.fullname = level.name;
		} else {
			level.fullname = ".";
		}
	}

	fprintf(stdout, "recur: %d, threads: %d\n", level.recursive, level.threads);

	int errct = 0, i = 0;

	DIR *current = opendir(level.fullname);
	if (!current) return 1;
	struct dirent *entry;

	int filecount = 1;
	thpool_t *thpool = NULL;

	if (level.threads > 1) {
		filecount = dir_count(current, level.fullname);
		if (level.depth == 0)
			fprintf(stdout, "Creating pool\n");
			thpool = thpool_init(level.threads);
	}

	File thread_files[filecount];

	while ((entry = readdir(current))) {
		if (entry->d_name[0] == '.') continue;

		thread_files[i] = level;
		thread_files[i].name = strdup(entry->d_name);
		asprintf(&thread_files[i].fullname, "%s/%s", level.fullname, entry->d_name);

		if (level.threads > 1) {
			fprintf(stdout, "Adding pool work\n");
			thpool_add_work(thpool, check_file, (void *)&thread_files[i]);
			fprintf(stdout, "Pool work added\n");
			++i;
		} else {
			check_file((void *)thread_files);
		}
	}

	closedir(current);
	fprintf(stdout, "Closed dir, exiting\n");

	if (level.threads > 1 && level.depth == 0) {
		fprintf(stdout, "Destroying pool\n");
		thpool_destroy(thpool, thpool_graceful);
	}

	return errct;
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

void * check_file(void *in)
{
	File *file = (File *)in;
	struct stat s;
	stat(file->fullname, &s);

	if (S_ISDIR(s.st_mode)) {
		if (file->recursive) {
			++file->depth;
			if (file->dir_action) {
				file->dir_action(*file);
			}
			dir_process_fn(*file);
		} else return NULL;
	} else if (S_ISREG(s.st_mode) && file->file_action) {
		file->file_action(*file);
	}

	free(file->fullname);
	free(file->name);

	return NULL;
}

void dir_check_output()
{
	struct stat st = {0};

	if (stat(output, &st) == -1) {
	    mkdir(output);
	}
}