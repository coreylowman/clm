clm
===

###Matrix Creation

```
A = [4:4] //creates a 4x4 matrix filled with 0's and stores in A
B = {1 2, 3 4} // creates 2x2 matrix with row 1 as {1 2} and row 2 as {3 4}
```

###Matrix Indexing
```
A = [4:4]
A[1, 2] //first row, second column
A[3, ] //third row
A[, 4] //fourth column
A[2..4, 4] //a [3:1] slice
A[, 2..3] //a [4:2] slice
A[2..3, 2..3] //a [2:2] submatrix slice
```

###For loops
```
for i in 0..5 do
  print i
end
=> 012345

A = [2:2]
for (i,j) in A do
  print i
  printl j
end
=>11
=>12
=>21
=>22
```

In addition, you can specify the delta of the range by specifying the second value:

```
for i in 5,-1..0 do
  print i
end
=> 543210
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
  for i in 1..size do
    I[i,i] = 1
  end
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
  q = ...
  ...
  T = ...
  ...
  return T
;
```

###Function Calling

```
//calling a function named foo that has 2 parameters and no return value
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
