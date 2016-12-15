fig
===

Grep (and find plus grep) is slow in Cygwin. This is a much faster, multithreading
implementation.

Usage:

  fig [-i] [-l] [-n] [-t <file type>] [path] pattern

where
  -i: Case insensitive
  -l: List files only
  -n: Print line number
  -t: File type

Build with make. Pre-compiled .exe for use in Cygwin can be found in the bin directory.

Known issues:
- Case insensitive search is not implemented yet.
