
## How does the toolchain work?

### Scenario 1
When compiling a program, the following steps should be performed to achieve the end result:
1. Process every source file with prime_preprocessor which outputs prime source files with preprocessor directives removed/applied.
2. Process every source file with prime_driver which outputs prime IR files that are used by other toolchain elements.
3. Process every IR file with prime_compiler which outputs binary files.

### Scenario 2
When interpreting a program, the following steps should be performed to achieve the end result:
1. Process every source file with prime_preprocessor which outputs prime source files with preprocessor directives removed/applied.
2. Process every source file with prime_driver which outputs prime IR files that are used by other toolchain elements.
3. Process every IR file with prime_interpreter.

### Scenario 3
When running the tests blocks in the code, the following steps should be performed to achieve the end result:
1. Process every source file with prime_preprocessor which outputs prime source files with preprocessor directives removed/applied.
2. Process every source file with prime_driver which outputs prime IR files that are used by other toolchain elements.
3. Process every IR file with prime_tester.

TODO: make a diagram that outlines these scenarios!
