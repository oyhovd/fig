fig
===

Grep (and find plus grep) is slow, especially in cygwin. This is a much faster, multithreading
implementation.

Usage:

  fig [-i] [-l] [-n] [-t <file type>] [path] pattern

where
  -i: Case insensitive
  -l: List files only
  -n: Print line number
  -t: File type

Build with make.
