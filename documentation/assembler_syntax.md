# Syntax notes

## Preprocessor

All preprocessor directives must start with a `%`. The following preprocessor directives are supported:
|directive|syntax|notes|
|:-------:|:----:|-----|
|`define`| See [define synatax](#define-syntax)| See [define notes](#define-notes) |
|`if`|`%if cond`|The text between the `%if` and the following `%else`, `%elif`, `%end` will only be assembled if `cond` is true.|
|`else`|`%else`|The text following the `%else` will be assembled if the condition of the previous `%if`/`%elif` was false|
|`elif`|`%elif cond`|The text between the `%elif` and the following `%else`, `%elif` or `%end` will be assembled only if the condition of the previous `%if`/`%elif` was false and `cond` is true.|
|`end`|`%end`|Ends a preprocessor sequence.|

### `%define`

#### define syntax

```reqvmasm
%define NAME %type integral <integral_literal>
%define NAME %type string <string_literal>
%define NAME %type instruction_list <list_of_instructions>
```

where:

* `<integral_literal>` is a number, such as 1, 0xDEADC0DE, or 0o531565
* `<string_literal>` is a string literal such as "foobar"
* `<list_of_instructions>` is a list of zero or more instructions, an instruction may appear on the same line as the define itself.

The above are called 'bodies'.

`%type` must be followed by either `integral`, `string`, or `instruction_list`, or otherwise the assembler will output an error, and code generation will not be ran. The assembler will *not* try to infer the type on its own.

A `%define` must be paired with an `%end` directive, which should appear on its own line.

#### define notes

The `%define` preprocessor directive creates a *macro*. Macros are replaced with their bodies **only** when their names are prefixed with `@`, otherwise the name is treated as a normal symbol.

`__counter__` is a special macro predefined by the preprocessor, which has its value incremented with each expansion, and whose initial value is 0.

## Label syntax

Labels will follow this syntax: `.label_name:`. Labels must be alphanumeric and should not start with digits(for now).
