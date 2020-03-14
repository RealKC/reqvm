# Syntax notes

## Preprocessor

All preprocessor directives must start with a `%`. The following preprocessor directives are supported:
|directive|syntax|notes|
|:-------:|:----:|-----|
|`define`|`%define NAME body`|will textually replace `NAME` with `body`. Must end with an `%end`|
|`if`|`%if cond`|The text between the `%if` and the following `%else`, `%elif`, `%end` will only be assembled if `cond` is true.|
|`else`|`%else`|The text following the `%else` will be assembled if the condition of the previous `%if`/`%elif` was false|
|`elif`|`%elif cond`|The text between the `%elif` and the following `%else`, `%elif` or `%end` will be assembled only if the condition of the previous `%if`/`%elif` was false and `cond` is true.|
|`end`|`%end`|Ends a preprocessor sequence.|

Macros defined it with `%define` must have their name prefixed with `@` in order to be expanded as macros.

`__counter__` is a predefined preprocessor macro that acts as a counter starting from 0, and incrementing with each expansion.

## Label syntax

Labels will follow this syntax: `.label_name:`. Labels must be alphanumeric and should not start with digits(for now).
