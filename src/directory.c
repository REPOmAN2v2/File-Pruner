#define _GNU_SOURCE		// asprintf()

#include <sys/stat.h>	// struct stat and stat()
#include <stdio.h>
#include <dirent.h> 	// dirent
#include <stdlib.h> 	// free

#include "directory.h"
#include "thpool.h"		// threads
#include "gc.h"			// garbage collection

static void dir_process_fn(File level);
static void dir_process_threaded_fn(File level, thpool_t *thpool);
static void dir_check_file_threaded(File *file, thpool_t *thpool);
static void dir_check_file(File *file);
static int dir_count(DIR *in, const char *path);

void dir_process_wrapper(File level)
{
	if (!level.fullname) {
		if (level.name) {
			level.fullname = level.name;
		} else {
			level.fullname = ".";
		}
	}

	if (level.threads > 1) {
		fprintf(stdout, "Creating pool\n");
		thpool_t *thpool = thpool_init(level.threads);
		dir_process_threaded_fn(level, (void *)thpool);
		thpool_destroy(thpool, thpool_graceful);
		gc_collect_garbage();
	} else {
		dir_process_fn(level);
	}
}

void dir_process_fn(File level)
{
	DIR *cur_dir = opendir(level.fullname);
	if (!cur_dir) {
		fprintf(stderr, "The directory (%s) does not exist\n", level.fullname);
		return;
	}
	struct dirent *entry;

	File current = {0};

	while ((entry = readdir(cur_dir))) {
		if (entry->d_name[0] == '.')
			continue;

		current = level;
		current.name = entry->d_name;
		asprintf(&current.fullname, "%s/%s", level.fullname, entry->d_name);

		dir_check_file(&current);
	}

	free(current.fullname);
	closedir(cur_dir);
}

void dir_process_threaded_fn(File level, thpool_t *thpool)
{
	int i = 0, filecount;

	DIR *cur_dir = opendir(level.fullname);
	if (!cur_dir) return;
	struct dirent *entry;

	filecount = dir_count(cur_dir, level.fullname);

	File *thread_files = malloc(sizeof (File) * filecount);

	while ((entry = readdir(cur_dir))) {
		if (entry->d_name[0] == '.')
			continue;

		thread_files[i] = level;
		thread_files[i].name = strdup(entry->d_name);
		asprintf(&thread_files[i].fullname, "%s/%s", level.fullname, entry->d_name);

		dir_check_file_threaded(&thread_files[i], thpool);
		++i;
	}

	closedir(cur_dir);
	gc_add_garbage(thread_files);
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

void dir_check_file_threaded(File *file, thpool_t *thpool)
{
	struct stat s;
	stat(file->fullname, &s);

	if (S_ISDIR(s.st_mode)) {
		if (file->recursive) {
			++file->depth;
			if (file->dir_action) {
				thpool_add_work(thpool, file->dir_action, (void *)file);
			}
			dir_process_threaded_fn(*file, thpool);
		}
	} else if (S_ISREG(s.st_mode) && file->file_action) {
		thpool_add_work(thpool, file->file_action, (void *)file);
	}
}

void dir_check_file(File *file)
{
	struct stat s;
	stat(file->fullname, &s);

	if (S_ISDIR(s.st_mode)) {
		if (file->recursive) {
			++file->depth;
			if (file->dir_action) {
				file->dir_action(file);
			}
			dir_process_fn(*file);
		}
	} else if (S_ISREG(s.st_mode) && file->file_action) {
		file->file_action(file);
	}
}

void dir_check_output(const char* output)
{
	struct stat st = {0};

	if (stat(output, &st) == -1) {
	    mkdir(output);
	}
}