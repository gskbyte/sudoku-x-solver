Sudoku X solver
===============

This is a simple program that tries to solve a sudoku-x as fast as possible. It was developed as a competition for the subject "Artificial Intelligence" of the University of Granada. The exercise was intended to be a competition for the fastest algorithm. 

This implementation was able to solve 100 Sudoku-X with different complexities in about 2 seconds on a Core 2 Duo in the year 2009.

How it works
------------

It solves an array with 81 positions representing a 9x9 matrix using the typical method.

How to compile and execute it
-----------------------------

```
$ make sudokux  
$ ./sudokux <representation>
```

The program returns a plain string with the solved sudoku. For example (0=empty cell):

We want to solve the following sudoku-x (this is an easy one):

```
9.. .51 73.  
1.7 398 2.5  
5.. .76 091  

81. 724 35.  
2.. 165 ..7  
.75 983 .12 

.21 537 ...  
758 649 123  
39. 812 57.
```

```
$ ./sudokux 900051730107398205500076091810724350200165007075983012021537000758649123390812570
962451738147398265538276491816724359293165847475983612621537584758649123394812572
```

License
-------

Public Domain, but if you find it useful, please send me an email :)
