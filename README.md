clm
===

(In Progress!)Compiler for matrix based programming language called clm

To Do:

1. code generation
2. optimization

###Matrix Creation

```
A = [4:4] //creates a 4x4 matrix and stores in A
B = {1 2, 3 4} // creates 2x2 matrix with row 1 as {1 2} and row 2 as {3 4}
```

###Matrix Indexing
```
A = [4:4]
A[0,1] //first row, second column
A[2,] //third row
A[,3] //fourth column

```

###Function Declarations

```
//a function with no parameters and no return type
\foo =
  print 1
;
```

```
// a function with an integer parameter and an integer return type
\foo num:int -> int =
  return num + 1
;
```

```
// a function taking an integer parameter and return a size by size identity matrix
\foo size:int -> [size:size] =
	I = [size:size]
	for i = 0 to size - 1 do
		I[i,i] = 1
	;
	return I
;
```

```
//a function taking an m by n matrix and return a size n column vector
\foo A[m:n] -> [n:1] =
  B = [n:1]
  // some code here...
  return B
;
```

```
//a function taking an m by n matrix and return an matrix with n rows and an unknown amount of columns
\foo A[m:n] -> [n:q] =
  ...
  q = (...)
  ...
  T = (...)
  ...
  return T
;
```

###Function Calling

```
//calling a function named foo that has 2 parameters
call foo(param1,param2)

//calling a function named foo with no arguments and a return value
A = foo()
```

###Printing

```
//printl prints the expression with a newline after it
print A
printl 1 * 2 - 3
printl {1 0,0 1} * B
print foo()
```
