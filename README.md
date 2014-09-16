File-Pruner
=========

This utility prunes unecessary data from supported filetypes. Right now it only
supports .big files (actually .wav files) because this is what I needed it for 
and I threw it together in an hour.

Launch it using `pruner /path/to/folder/containing/.big/files/` (note the
trailing slash). It outputs the pruned files and their old unnecessary header
in `./output`.

It should work fine on all modern OS.

File support
-----------

If you want to modify this to support another format, it's easy as long as the 
format is organised in mostly the same way and you want to delete the header.

You'll have to modify the chunkID, which is the pattern you want to recognise.
Everything up to that chunk is deleted. Then, modify the (terrible) file
extension detection in checkFormat(). Finally, depending on the length of your
chunk you'll have to modify the way the chunk recognition code is built when
readingthe file, in editFile(). Right now, it reads 4 bytes byte-by-byte and
stores them in an uint32.

This will probably be updated in the future to support other filetypes natively.

Version
----

0.2

Installation
--------------

##### Requires MinGW on Windows
`make`

License
----

MIT