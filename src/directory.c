#define _GNU_SOURCE		// asprintf()

#include "directory.h"
#include <sys/stat.h>	// struct stat and stat()
#include <stdio.h>
#include <dirent.h> 	// dirent
#include <stdlib.h> 	// free

int processDirFn(File level)
{
	if (!level.fullname) {
		if (level.name) {
			level.fullname = level.name;
		} else {
			level.fullname = ".";
		}
	}

	int errct = 0;

	DIR *current = opendir(level.fullname);
	if (!current) return 1;
	struct dirent *entry;

	while ((entry = readdir(current))) {
		if (entry->d_name[0] == '.') continue;

		File next_level = level;
		next_level.name = entry->d_name;
		asprintf(&next_level.fullname, "%s/%s", level.fullname, entry->d_name);

	#ifdef _WIN32
		struct stat s;
		stat(next_level.fullname, &s);
		if (S_ISDIR(s.st_mode))
	#else
		if (entry->d_type == DT_DIR)
	#endif
		{
			if (level.flag) {
				++next_level.depth;
				if (level.dirAction) {
					level.dirAction(next_level);
				}
				errct += processDirFn(next_level);
			} else continue;
		}
	#ifdef _WIN32
		else if (S_ISREG(s.st_mode) && level.fileAction)
	#else
		else if (entry->d_type == DT_REG && level.fileAction)
	#endif
		{
			level.fileAction(next_level);
			errct+= next_level.error;
		}
		free(next_level.fullname);
	}

	closedir(current);

	return errct;
}