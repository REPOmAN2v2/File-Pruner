File-Pruner
=========

This utility prunes unecessary data from supported filetypes. Right now it only
supports .big files (actually .wav files) because this is what I needed it for 
and I threw it together in an hour.

Launch it using `pruner -p /path/to/folder/containing/files/ -e extension` (note
the trailing slash). The extension is, for example, `big` or `wav`. It outputs 
the pruned files and their old unnecessary header in `./output`.

Supported command line arguments are: 

* `--version` or `-v`
* `--help` or `-h`
* `--path` or `-p` (required)
* `--extension` or `-e` (required)

It should work fine on all modern OS.

File support
-----------

If you want to modify this to support another format, it's easy as long as the 
format is organised in mostly the same way and you want to delete the header.

You'll have to modify the chunkID, which is the pattern you want to recognise.
Everything up to that chunk is deleted. Depending on the length of your
chunk you'll have to modify the way the chunk recognition code is built when
reading the file, in editFile(). Right now, it reads 4 bytes byte-by-byte and
stores them in an uint32.

This will probably be updated in the future to support other filetypes natively.

Version
----

0.3

Installation
--------------

##### Requires MinGW on Windows
`make`

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
