Requirements to RAM:
    For double 8 byte * 1 500 000 000 / (1024 * 3) = 11.18Gb
    For double 8 byte * 1 000 000 000 / (1024 * 3) = 7.45Gb
    For float  4 byte * 1 500 000 000 / (1024 * 3) = 5.58Gb
    For float  4 byte * 1 000 000 000 / (1024 * 3) = 3.72Gb
    
    
Requirements to the size of the lookup table:
    For double 8 byte: 600 000 000, 800 000 000, 1 000 000 000, 
                       1 200 000 000, 1 400 000 000
                       
    For float 4 byte:  600 000 000, 800 000 000, 1 000 000 000, 
                       1 200 000 000, 1 400 000 000
                   
                   
COMPILER OPTION:
Compiler options for the linear version:


Compiler options for the vectorize version:


Compiler options for the autovectorize version: