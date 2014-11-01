File-Pruner
=========

This utility prunes unecessary data from files.

Launch it using at least `pruner -i string` (don't add a trailing slash to the path). The string should be the pattern to recognise, for example `RIFF` for `wav` files.

The program supports multithreading, so files can be parsed and processed in parallel.

Supported command line arguments are: 

* `--version` or `-v`
* `--help` or `-h`
* `--path` or `-p` (defaults to `.` i.e. the current directory)
* `--id` or `-i`: the pattern to recognise, should be a regular string, not hexadecimal (required)
* `--extension` or `-e`: the program can search files with a specific extension only
* `--recursive` or `-r`: recursively parse the directory i.e. search in subfolders
* `--threads` or `-t`: specify the number of threads (defaults to `1`)

File formats
-----------

This program can be used against any file as long as you give it a pattern to recognise. Everything before the first occurence of that pattern will be deleted. 

Implementation
------------

This program started as a simple 100-line script to strip headers from files. It worked very well but was limited in scope and not very safe It has now become more of a pet project to learn file processing and multithreading.

The code is fairly well encapsulated. The directory parser is self contained and takes any number of struct _file members as arguments thanks to a simple macro. Two of these arguments can be callbacks to execute when encountering a dir or a file. Directories can be recursively searched if the flag is set. 

The file processor is built around the file structure returned from the dir parser and is also self-contained. It is run in individual threads if the user desires, so all the file IO and processing happens in parallel. Files are processed in chunks, which is safer than loading the entire file in memory, however big it is, at the cost of a small performance hit. Further experimentation is needed to find if the chunk size matters a lot. The pattern is searched for using a simple algorithm for now.

Threading uses a thread pool and a basic garbage collector because of the recursive search of folders. Indeed, the directory parser allocates memory to pass to the threads and freeing this memory when returning from a recursive call would corrupt the thread's memory, since they run independently. The GC is simply a double linked list containing a pointer to the array of thread data which should be freed when all the threads are done working. The data buffer contained in those file structures is freed independently in each thread. 

All credit and ownership goes to [Pithikos](https://github.com/Pithikos/C-Thread-Pool) for the LGPL implementation of the thread pool, though I made a few modifications. By default, no threads are created and the program runs serially. File IO is very expensive so having multiple threads write to a disk isn't necessarily better. However this will depend on your hard drive setup, your CPU, your OS, the number and the size of the files as well as the number of threads you create, etc. 

Version
----

1.0

Compilation
--------------

##### Requires MinGW on Windows
`make` for a release build or `make DEBUG=1` to get debugging symbols and `gprof` profiling.

License
----

The MIT License (MIT)

Copyright (c) 2014 REPOmAN2v2

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
