#define _GNU_SOURCE		// asprintf()

#include "directory.h"
#include "include.h"	// global constants
#include <sys/stat.h>	// struct stat and stat()
#include <stdio.h>
#include <dirent.h> 	// dirent
#include <stdlib.h> 	// free
#include <pthread.h>	// threads

static int dir_count(DIR *in, const char *path);
static void * thread_check_file(void *in);

int dir_process_fn(File level)
{
	if (!level.fullname) {
		if (level.name) {
			level.fullname = level.name;
		} else {
			level.fullname = ".";
		}
	}

	int errct = 0, i = 0;

	DIR *current = opendir(level.fullname);
	if (!current) return 1;
	struct dirent *entry;

	int filecount = dir_count(current, level.fullname);
	pthread_t threads[filecount];
	File thread_files[filecount];

	while ((entry = readdir(current))) {
		if (entry->d_name[0] == '.') continue;

		thread_files[i] = level;
		thread_files[i].name = strdup(entry->d_name);
		asprintf(&thread_files[i].fullname, "%s/%s", level.fullname, entry->d_name);

		int rc = pthread_create(&threads[i], NULL, thread_check_file, (void *)&thread_files[i]);

		if (rc) {
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(EXIT_FAILURE);
		}
		++i;
	}

	closedir(current);
	for (int i = 0; i < filecount; ++i) pthread_join(threads[i], NULL);

	return errct;
}

/**
 *	Really annoying having to do that, consider implementing a thread pool in
 *	the future
 */
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

void * thread_check_file(void *in)
{
	File *file = (File *)in;
	struct stat s;
	stat(file->fullname, &s);

	if (S_ISDIR(s.st_mode)) {
		if (file->flag) {
			++file->depth;
			if (file->dir_action) {
				file->dir_action(*file);
			}
			dir_process_fn(*file);
		} else pthread_exit(NULL);
	} else if (S_ISREG(s.st_mode) && file->file_action) {
		file->file_action(*file);
	}

	free(file->fullname);
	free(file->name);
	pthread_exit(NULL);
}

void dir_check_output()
{
	struct stat st = {0};

	if (stat(output, &st) == -1) {
	    mkdir(output);
	}
}