File-Pruner
=========

This utility prunes unecessary data from supported filetypes. Right now it only supports .big files (actually .wav files) because this is what I needed it for and I threw it together in an hour.

Launch it using at least `pruner -e extension` (don't add a trailing slash to the path). The extension is, for example, `big` or `wav`. It outputs the pruned files and their old unnecessary header in `../output`.

The program supports multithreading, so files can be parsed and processed in parallel.

Supported command line arguments are: 

* `--version` or `-v`
* `--help` or `-h`
* `--path` or `-p` (defaults to `.` i.e. the current directory)
* `--extension` or `-e` (required)
* `--recursive` or `-r`: recursively parse the directory i.e. search in subfolders
* `--threads` or `-t`: specify the number of threads (defaults to `1`)

`-t` and `-r` are not currently compatible.

File formats
-----------

If you want to modify this to support another format, it's easy as long as the format is organised in mostly the same way and you want to delete the header.

You'll have to modify the chunkID in file.c, which is the pattern you want to recognise. Everything up to that chunk is deleted. Depending on the length of your chunk you'll have to modify the way the chunk recognition code is built when reading the file, in file_find_chuck(). Right now, it reads 4 bytes byte-by-byte and stores them in an uint32.

This may be updated to natively support other formats in the future.

Implementation
------------

The code is fairly well encapsulated. The directory parser is self contained and takes any number of struct _file members as arguments thanks to a simple macro. Two of these arguments can be callbacks to execute when encountering a dir or a file. Directories can be recursively searched if the flag is set. 

The file processor is built around the file structure returned from the dir parser and it's also self-contained. The skeleton of a rudimentary error accounting and checking system has been added to it. 

Threading uses a thread pool. All credit and ownership goes to [Pithikos](https://github.com/Pithikos/C-Thread-Pool) for the LGPL implementation. By default, no threads are created and the program runs serially. File IO is very expensive so having multiple threads write to a disk isn't necessarily better. However this will depend on your hard drive, your CPU, the number and the size of the files as well as the number of threads you create. Threading as implemented won't play nicely with the recursive search either.

Version
----

0.8

Installation
--------------

##### Requires MinGW on Windows
`make` for a release build or `make DEBUG=1` to get debugging symbols and `gprof` profiling. Note that profiling with `gprof` seems very expensive when multithreading.

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
