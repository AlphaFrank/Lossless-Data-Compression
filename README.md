# Lossless-Data-Compression
A lossless data compression software implemented using the LZ77 data compression algorithm.

To compile LZ.cpp:
g++ LZ.cpp -o LZ -std=c++0x
To run LZ: 
./LZ [optional parameter values] [file name]

To compile EXPAND.cpp:
g++ EXPAND.cpp -o EXPAND -std=c++0x
To run EXPAND: 
./EXPAND [optional file name]

LZ is an executable to compress files. LZ needs a required file name as the input file name. It outputs the compressed files to the standard output stream. It is optional to modify parameters. There are three parameters. N decides the size of the sliding window. L decides the size of the lookahead buffer. S decides the length of the longest character sequence to be encoded into one token. The sliding window are implemented as a hash table instead of an array for quick search purpose. Any error massages are directed to the standard error stream. 

EXPAND is an executable to decompress compressed files. It takes one optional parameter, the name of input file. If none is specified, it takes input from the standard input stream. It outputs the decompressed files to the standard output stream.

