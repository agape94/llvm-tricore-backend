# LLVM Backend for the TriCore TC1.6 architecture
> **Note**: This backend is based on the old LLVM TriCore TC1.3 backend. The code can be found [in this GitHub repo](https://github.com/TriDis/llvm-tricore).

> **WARNING:** The functionality of the compiler was NOT tested in a comprehensive way. Please check the assembly code before running it on your hardware! 

> **Note:** The relevant code for this backend can be found in [this folder](llvm/lib/Target/TriCore/).

This project aims to provide an open source, LLVM-based compiler for the `TriCore TC1.x` architecture. The instructions and register definitions as well as instruction selection implementations are based on the old LLVM TriCore TC1.3 backend that was developed in 2016. You can find the repository [here](https://github.com/TriDis/llvm-tricore).

## Build
### Build command
To start a `Release` build, simply run
```
$ inv b
``` 
in your command line. Depending on your hardware, it may take up to 1h.

### Details

This project makes use of the excelent [invoke](https://www.pyinvoke.org/) module from Python. It allows the creation of custom scripts, called `tasks`, that can be run from anywhere within the directory where `tasks.py` exists (subdirectories as well). Invoke can be installed via Pip (`pip install invoke`).

To get an overview of the available tasks, open the directory in the command line and run the following command:
```
$ invoke --list
Available tasks:

  build-target (b)   Starts the build process for the target specified in the target_name parameter. If the target_name is not provided, the
                     name of the target the environment was created for will be used.
```
>**Note**: `inv` works as well.

For more information about Python `invoke`, please refer to the [official documentation](https://docs.pyinvoke.org/en/stable/).

To find more information about a specific task, you can run the following command: `inv --help [task_name]`. This command will print the usage instructions of the task, the task description, followed by detailed descriptions of all command line arguments.

For instance, running `inv --help b`, you will get the following output:
```
inv --help b
Usage: inv[oke] [--core-opts] b [--options] [other tasks here ...]

Docstring:
  Starts the build process for the target specified in the target_name parameter. If the target_name is not provided, `TriCore` will be used by default.
  By default, the build type is set to Release. If you want to build the target in Debug mode, you can specify the build type as Debug.

Options:
  -a INT, --parallel-link-jobs=INT           Number of parallel link jobs. By default it is set to 2. WARNING: A high number of link jobs may cause your system to run out of memory and crash (Especially for Debug builds).
  -b STRING, --build-type=STRING             Possible values: Debug, Release. Default value: Release
  -c, --[no-]clang                           Whether to build clang or not. Default value: True
  -e STRING, --experimental-targets=STRING   List of experimental targets to build, Strings separated by ';' (semicolon). Example: 'TriCore;RISCW;etc'. If not defined, the name of the target the environment was created for will be used.
  -p STRING, --path=STRING                   Path to llvm source root. By default the current directory is considered
  -u STRING, --upstream-targets=STRING       List of upstream targets to build, Strings separated by ';' (semicolon). By default it is None. Example: Lanai;ARM;RISCV;etc
```
To trigger a `Debug` build, simply run 

```
inv b -b Debug
```

## Generate assembly code

If the build is successful, a new directory will be created inside the environment directory. This directory will be called `build_<target-name>`, and will contain all the binaries generated, including `llc`, which is the actual compiler we are interested in.

As input, `llc` needs `LLVM IR` source code. Create a new file `main.ll` and place the following code inside.
```
; ModuleID = 'Example.c'
source_filename = "/home/alex/temp/llvm-simple-test/example.c"
target datalayout = "e-m:e-p:32:32-i64:64-n32-S128"
target triple = "tricore"

; Function Attrs: noinline nounwind optnone
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  store i32 100, ptr %2, align 4
  %4 = load i32, ptr %2, align 4
  %5 = add nsw i32 %4, 300
  store i32 %5, ptr %3, align 4
  %6 = load i32, ptr %2, align 4
  %7 = load i32, ptr %3, align 4
  %8 = add nsw i32 %6, %7
  ret i32 %8
}
```
The equivalent C code for the `LLVM IR` code above is the following:
```
int main()
{
        int a;
        a = 100;
        int b = a + 300;
        return a+b;
}
``` 

In order to generate the assembly code for this program, `llc` can be run as follows:

```
$ ./build_tricore_<build_type>/bin/llc --mtriple=tricore /path/to/src.ll
```

Note: 
  - The `--mtriple` flag tells `llc` for which target the code will be compiled. In our case it is `<target-name>`.
  - The `.s` file containing the assembly code will be placed alongside the `.ll` file.

The generated assembly code for the `LLVM IR` code above is the following:
```
        .text
        .file   "variables.c"
        .globl  main                            # -- Begin function main
        .type   main,@function
main:                                   # @main
main$local:
        .type   main$local,@function
# %bb.0:
        mov %d15, 0
        st.w [%a14] -4, %d15
        mov %d15, 100
        st.w [%a14] -8, %d15
        ld.w %d15, [%a14] -8
        mov %d2, 300
        add %d2, %d15
        st.w [%a14] -12, %d2
        ld.w %d15, [%a14] -8
        ld.w %d2, [%a14] -12
        add %d2, %d15
        ret
.Lfunc_end0:
        .size   main, .Lfunc_end0-main
        .size   main$local, .Lfunc_end0-main
                                        # -- End function
        .ident  "clang version 17.0.6"
        .section        ".note.GNU-stack","",@progbits
```

## Binary code
For now, the assembler is not implemented. Please use an existing assembler to generate the binary.
