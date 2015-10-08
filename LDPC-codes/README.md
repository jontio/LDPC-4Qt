This is almost a copy of radfordneal/LDPC-codes as of 8/10/15.
There are two bug fixes I've done as far as I remember both memory leaks.
One in distrib_create and the other in mod2sparse_free, both I've signed.
The name of this file I've changed from README to README.md. I've placed
header gards around all the header files. Also I have cast to the type of
variables returned by chk_alloc as it would give me an error otherwise.
In rand.c I changed two declarations to work with C++ toward the end of
the file and also removed the RAND_FILE definition for a char* with a
setRandFileName function in rand.c.

These files do the main work behind the Qt C++ wrapper.

Jonti

-----------------------------

This is a collection of programs and modules, written in C, that
support research and education concerning Low Density Parity Check
(LDPC) codes.

See index.html in this directory for an index to the documentation.
Copyright information can be found there, and in the file COPYRIGHT.
