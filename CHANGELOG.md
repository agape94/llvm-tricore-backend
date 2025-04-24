# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [v0.1.0]
This is the first public release of the TriCore backend.
This version includes the core structure of the backend and has all the core parts implemented. It is very much in an alpha state, so expect bugs and missing features. However, contributions and feedback are welcome!

### Added
- TableGen definitions for all instruction formats, instructions and registers
- All the functionality related to instruction selection, register allocation, DAG lowering and legalization, etc.

### Known issues
- Function pointer arguments are not supported
- Register allocation fails for function calls, which may be due to incomplete or incorrect support for the calling convention, or another (unknown) underlying issue
- Switch statements are not supported

