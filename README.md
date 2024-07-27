# Wacky Editor
A text editor project intended for me to
learn and explore the different components
and technologies that goes into making a
text editor.

# About
The text editor in its current form is NOT complete.
There are several known issues that need to be addressed.
This current version is meant to show off what I have so far
and the optimizations that I have made (mainly so I can put
it on my resume).

# Features
The text editor is able to handle text editing (obviously) which includes:
writing text and backspacing to delete text. It also has basic syntax highlighting
for keywords for the C language, single line comments, multi-line comments, and strings.
It is optimized to be able to handle large files. The largest file I have tested is
a little over 2 mb in size (around 100,000 lines of code).

Do note: The text editor is optimized for code files with relatively short rows
(like < 1000 characters per row). Therefore the text editor may struggle to handle
files with very long lines.

# Known Bugs
The text editor is not in its complete state, listed below are the known bugs:
 - none found at the moment

## How to use
Install ncurses
 - MAC: `brew install ncurses`

Compile the code
 - `make all`

Run the text editor
 - `./Wacky`

## Status
Still under development ...
Check development branch to see the progress.
