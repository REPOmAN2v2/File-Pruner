#include <stdlib.h>		// malloc, free
#include "gc.h"
#include "directory.h"	// struct _file

typedef struct _gc Garbage;

struct _gc {
	File *file;
	Garbage *next;
	Garbage *prev;
};

static Garbage ** gc_get_dump()
{
	static Garbage *dump;

	return &dump;
}

void gc_add_garbage(File *file)
{
	Garbage **dump = gc_get_dump();
	Garbage *trash = malloc(sizeof (Garbage));

	trash->file = file;

	if (*dump) {
		(*dump)->prev->next = trash;
		trash->prev = (*dump)->prev;
		trash->next = *dump;
		(*dump)->prev = trash;
	} else {
		*dump = trash;
		trash->next = trash->prev = trash;
	}
}

void gc_collect_garbage()
{
	Garbage **dump = gc_get_dump();

	if (!(dump && *dump))
		return;

	while (*dump) {
		Garbage *trash = *dump;
		trash->next->prev = trash->prev;
		trash->prev->next = trash->next;

		*dump = (trash != trash->next) ? trash->next : NULL;

		free(trash->file->fullname);
		free(trash->file->name);
		free(trash->file);
		free(trash);
	}
}