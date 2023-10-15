
# Prime
**High Level Design (HLD) Document**

## Project Overview
This document provides a high-level overview of the "prime" programming language project, its goals, and the architectural design of the toolchain.

### Purpose
The "prime" programming language project aims to create a type-safe, memory-safe, and low-level programming language that combines the best elements of Python, C, and Rust. This language is designed for simplicity, readability, and low-level system programming.

### Goals
 - **Type Safety:** - enforce strict type checking to prevent common programming errors.
 - **Memory Safety:** - implement Rust-inspired memory safety features to prevent memory-related bugs.
 - **Simplicity:** - prioritize a simple and easily understandable syntax inspired by Python.
 - **Low-Level Capabilities:** - allow low-level system programming akin to C.

[(to the top)](#prime)

## Language Design Principles
 - **Immutability by Default:** - all variables are immutable by default and must be explicitly marked as mutable.
 - **Primitives:** - include fundamental data types and operations.
 - **Control Structures:** - provide conditional blocks (if, elif, else), loops (while, for), and matching blocks (switch, match).
 - **No Modules:** - use preprocessor directives for code organization.
 - **Comments:** - support comments for code documentation and readability.

[(to the top)](#prime)

## Toolchain Components
The "prime" programming language is supported by a comprehensive toolchain consisting of the following components:
 - **prime_compiler:** - compiles "prime" code into native binaries.
 - **prime_decompiler:** - decompiles native binaries into "prime" code.
 - **prime_driver:** - creates intermediate representation (IR) files from source code.
 - **prime_interpreter:** - interprets "prime" IR files.
 - **prime_preprocessor:** - preprocesses source files, handling includes and macros.
 - **prime_tester:** - executes test blocks within IR files, inspired by Rust's in-code tests.

[(to the top)](#prime)

## Directory Structure
The project is organized into the following directory structure:
 - **examples:** - contains sample code and usage examples.
 - **libraries:** - houses common libraries and dependencies that are used by toolchain components.
 - **toolchain:** - holds the various toolchain components.

[(to the top)](#prime)

## Development Environment
 - **Languages:** - primarily developed in C language.
 - **Tools:** - utilizes ( TODO: add building system here! ).
 - **Platforms:** - target platforms include unix systems that support linux kernel.

[(to the top)](#prime)
