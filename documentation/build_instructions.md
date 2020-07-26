# Build instructions

The reqvm projects consists of two programs, the reqvm virtual machine and the reqvm assembler.

Note that this guide assumes you're in the root of the repository.

## Building the virtual machine

```sh
$ make -C ./vm
```

You may optionally specify:

*`DEBUG`(`yes|no`) - turns off optimizations and adds debug symbols. By default the value  is `yes`.

The binary will be under ./vm by the name vm (with the platform extension suffix if needed).

## Building the assembler

```sh
$ make -C ./assembler
```

You may optionaly specify:

* `DEBUG`(`yes|no`), by default the value is `yes`
* `AGGRESIVE_LOGGING`(`yes|no`), by default the value is `yes`. This option makes the assembler log all function call parameters. Please note that this is very verbose.

The assembler depends on the [magic_enum](https://github.com/Neargye/magic_enum/) library and it is included as a git submodule(under ./assembler/thirdparty), you may clone recursively(`git clone --recursive`), init the submodules yourself(`git submodule init`), or do nothing, the Makefile should be able to take care of it.

The binary will be under ./assembler by the name assembler (with the platform extension suffix if needed).
