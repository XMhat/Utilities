# Command-Line Calculator

A simple utility to perform calculations and advanced calculations from the command-line with help from the *LUA* interpreter.

## Usage
`clc <expression>`

Where `<expression>` is a typical *LUA* accepted syntax.

Specify no expression to get the version information and date.

## Advanced

The `math` namespace is also exposed to the global namespace so the following advanced commands are easily accessable.

| Function | Purpose |
|---|---|
| abs(x) | Returns the absolute value of `x`. |
| acos(x) | Returns the arc cosine of `x` (in *radians*). |
| asin(x) | Returns the arc sine of `x` (in *radians*). |
| atan(x) | Returns the arc tangent of `x` (in *radians*). |
| ceil(x) | Returns the smallest integer larger than or equal to `x`. |
| cos(x) | Returns the cosine of `x` (assumed to be in *radians*). |
| deg(x) | Returns the angle `x` (given in *radians*) in *degrees*. |
| exp(x) | Returns the value `e` power `x`. |
| floor(x) | Returns the largest integer smaller than or equal to `x`. |
| fmod(x, y) | Returns the remainder of the division of x by y that rounds the quotient towards zero. |
| huge | The value `HUGE_VAL`, a value larger than or equal to any other numerical value. |
| log(x) | Returns the natural logarithm of `x`. |
| max(x, ...) | Returns the *maximum* value among its arguments. |
| maxinteger | Returns the *maximum* value of a `lua_Integer` type (typically 64-bits). |
| min(x, ...) | Returns the *minimum* value among its arguments. |
| mininteger | Returns the *minimum* value of a `lua_Integer` type (typically 64-bits). |
| modf(x) | Returns two numbers, the *integral* part of `x` and the *fractional* part of `x`. |
| pi | The value of *pi*. |
| rad(x) | Returns the angle x (given in `degrees`) in `radians`. |
| random([m [, n]]) | This function is an interface to the simple pseudo-random generator function rand provided by ANSI C. When called without arguments, returns a uniform pseudo-random real number in the range `0` to ``1`. When called with an integer number `m`, `random` returns a uniform pseudo-random integer in the range `1` to `m`. When called with two integer numbers `m` and `n`, `random` returns a uniform pseudo-random integer in the range `m` to `n`. |
| randomseed(x) | Sets `x` as the *seed* for the pseudo-random generator: equal seeds produce equal sequences of numbers. |
| sin(x) | Returns the sine of `x` (assumed to be in `radians`). |
| sqrt(x) | Returns the square root of `x`. (You can also use the expression `x^0.5` to compute this value.) |
| tan(x) | Returns the tangent `o`f x (assumed to be in *radians*). |
