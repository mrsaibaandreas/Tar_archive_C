#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

//the define below set all those permissions in one variable
#define S_IRWURWGRWO (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) 
#define BUFF_SIZE 512 //the buffer size

typedef struct tar_header{
    char name[100];//name of the file
    char mode[8];//file mode
    char uid[8];//file uid
    char gid[8];//file gid
    char size[12];//file size
    char mtime[12];//last modification
    char checksum[8];//checksum of the file
}header;

void write_tar(char *file, int fd)
{
    header tar_header;
    struct stat my_file;//stat struct 
    stat(file, &my_file);//pointing stat to our file

    memset(&tar_header, 0, sizeof(header));//setting the memory to 0
    strncpy(tar_header.name, file, 100);//writing in tar_header the name
    snprintf(tar_header.mode, sizeof(tar_header.mode), "%07o", my_file.st_mode & 0x777);//writing the mode
    snprintf(tar_header.uid, sizeof(tar_header.uid), "%07o", my_file.st_uid);//writing the uid
    snprintf(tar_header.gid, sizeof(tar_header.gid), "%07o", my_file.st_gid);//writing the gid
    snprintf(tar_header.size, sizeof(tar_header.size), "%011o", (int)my_file.st_size);//writing the size
    snprintf(tar_header.mtime, sizeof(tar_header.mtime), "%011o", (int)my_file.st_mtime);//writting the time
    memset(tar_header.checksum, ' ', 8);//setting the memory of checksum to space

    int checksum = 0;
    unsigned char *checksum_string =  &tar_header;//writing the tar_header into a string
    for(int i = 0; i < sizeof(header); i++)
    {
        checksum += checksum_string[i];//computing the bytes in the checksum
    }

    snprintf(tar_header.checksum, sizeof(tar_header.checksum), "%06o", checksum);//writting the checksum

    write (fd, checksum_string, sizeof(tar_header));//writting the struct into the tar file
    int fa = 0;//fa = file to be archived 
    if ((fa = open(file, O_RDONLY)) < 0)//open the file
    {
        perror(file);
        exit(-1);
    }
    int r = 0;//the returned value of write see below
    char buffer[BUFF_SIZE] = {};//empty buffer, where we read the content of the file, which we wish to archive
    long int size_buffer = 512 - sizeof(header);//the bytes that we need to pad in the archive
    char empty_buffer[size_buffer];//empty array which we want to add as padding
    memset(&empty_buffer, 0, size_buffer);//setting the memory to 0
    write(fd, empty_buffer, size_buffer);//writting into the tar file
    while ( (r = read(fa, buffer,512)) > 0)//reading the file we want to archive
    {	write(fd, buffer, r);
        if (r != 512)//if we read the last chunck of files, than we need to complete with the padding
	{
		memset(&empty_buffer, 0, 512-r);
		write(fd, empty_buffer, 512-r);
	}
    }
    close(fa);//close the file we want to archive

}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        perror("Too few arguments\n");
        exit(-1);
    }

    int fd = 0;
    if ((fd = open(argv[2], O_WRONLY | O_CREAT | O_EXCL | O_APPEND, S_IRWURWGRWO)) < 0)//open the archive
    {
        perror(argv[2]);
    }

    write_tar (argv[1], fd); //write the archive

    close(fd);//close the archive
    return 0;
}
