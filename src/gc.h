#ifndef GC_H_
#define GC_H_

#include "directory.h"

void gc_add_garbage(File *file);
void gc_collect_garbage();

#endif