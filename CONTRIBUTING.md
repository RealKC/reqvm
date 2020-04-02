# Contributing guidelines

## Issues

When creating an issue to report a bug please include as much information as possible and make sure it's an actual bug in my code instead of one in your program. Include steps to reproduce the bug.

## Pull requests

Do:  

* submit code that uses C++17 features
* format your code using clang-format and the provided .clang-format file
* make sure to not cross the 72-column mark for each line in your commit messages

Do not:

* submit unformatted code
* write in C
* submit code that triggers compiler warnings or does not compile

Naming conventions: `snake_case` must be used for functions, names of types and namespaces. `CAPITAL_SNAKE_CASE` must be used for macros. Private variables must have their names starting with an underscore(`_`).
