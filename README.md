# myls
This repository contains my implementation of ls command with -latucifd1 options in C language.
It lists information about the files (the current directory by default), sorts them alphabetically if none of -cftu is
specified.
## supported options
- `-l` : list files in long format.
- `-a` : show hidden files.
- `-t` : sort by modification time (newest first).
- `-u` : with `-lt` :  sort by, and show, access time; with `-l`: show access
              time and sort by name; otherwise: sort by  access  time,  newest
              first.
- `c` :  with `-lt` : sort by, and show, ctime (time of last modification of
              file status information); with `-l`: show ctime and sort by  name;
              otherwise: sort by ctime, newest first.

if more than one sorting option were given, `-u` and `-c` have higher priority than `-t` , if both were given, sort by last one.

- `i` : show inode number.
- `f` : do not sort, enable `-a`, disable `-l` if `-i` not spicified.
- `d` : list directories themselves, not their contents. this option has highest priority.
- `1` : list one file per line.

### other features
- List entries of more than one directory.
- Supports any combination of provided options.
- Colored output (directories in blue, executables in green and other files in default stdout color).

## usage
clone the repo then compile source files using: 
```
make
```
Then execute the program using:
```
./myls <any combination of provided options>
```
## Sample output
![image](https://github.com/user-attachments/assets/217a9d34-ab68-47a7-8f20-57fe0c1195c0)

![image](https://github.com/user-attachments/assets/89f8544d-080b-40ab-9afd-87edbb3f9012)

video showing output: [https://drive.google.com/file/d/1dkU7ZiTVUYYCNpOrhlbI6ZwkZBYkJ53p/view?usp=drive_link](https://drive.google.com/file/d/1UclV2qAD6Cfjj_hotE7LXxPz6Msw84S5/view?usp=drive_link)


  
