# Tar_archive_C
A program which stores a file into a tar archive
At the moment the program can archive a file only

How to use it:
gcc -Wall -o <name_of_executable> tar.c


./<name_of_executable> <file_to_be_archived> <archive_name>
or
./<name_of_executable> <files_to_be_archived> <archive_name>

The second command should have more then 1 file as arguments

how to check if it works(here we dearchive the file)
tar -xfv <archive_name>


Thank you for checking this repo :D
