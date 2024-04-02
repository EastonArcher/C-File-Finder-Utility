# File Finder Utility
This repository contains a simple file finder utility implemented in C. The utility mimics some functionalities of the find command available in Unix-like operating systems. It allows users to search for files in a specified directory and its subdirectories based on various criteria such as file name, modification time, inode number, and perform actions like deletion on the matched files.

### Compilation
To compile the utility, ensure you have gcc installed. Then, run the following command: `make`

### Usage
The utility provides several command-line options:
- ***-w directory***: Specify the directory to start the search from.
- ***-n name***: Search for files with the specified name.
- ***-m mmin***: Search for files modified within the last mmin minutes. You can use + or - prefix to specify greater than or less than mmin minutes respectively.
- ***-i inum***: Search for files with the specified inode number.
- ***-a action***: Specify the action to perform on the matched files. Currently, only delete action is supported.
<br>
Here is an example of how to use the utility:
<br>
``./find -w /path/to/search/directory -n filename.txt -m +5 -a delete``
<br>

***(School Project)***

