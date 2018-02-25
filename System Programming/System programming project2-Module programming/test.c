#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
 
#include "scull_ioctl.h"

int main(int argc, char *argv[])
{
    char *file_name = "/dev/test";
    int read_mode = 0;
    char *user="root";
    
    int fd;enum
    {
		change_read_mode,
        delete_messages,
        e_set
    } option;
 
	if (argc == 4)
    {
		file_name = argv[1];
		
        if (strcmp(argv[2], "-r") == 0)
        {
            option = change_read_mode;
            read_mode = atoi(argv[3]);
        }
        else if (strcmp(argv[2], "-d") == 0)
        {
            option = delete_messages;
            user = argv[3];
        }
        else if (strcmp(argv[2], "-s") == 0)
        {
            option = e_set;
        }
        else
        {
            fprintf(stderr, "Usage: %s [ filename | -r [0-1] | -d [username]\n", argv[0]);
            return 1;
        }
    }
    else
    {
        fprintf(stderr, "Usage: %s [ filename | -r [0-1] | -d [username]\n", argv[0]);
        return 1;
    }
    
    fd = open(file_name, O_RDWR);
    printf("fd: %d\n",fd);
    if (fd == -1)
    {
        perror("query_apps open");
        return 2;
    }      
	
 
    switch (option)
    {
        case change_read_mode:            
			if (ioctl(fd, READ_MODE,read_mode) == -1)
			{
				perror("set read mode: ");
			}
            break;
        case delete_messages:  
			if (ioctl(fd, DELETE_MESSAGES,user) == -1)
			{
				perror("delete messages: ");
			}
            break;
        case e_set:
            break;
        default:
            break;
    }
 
    close (fd);
 
    return 0;
}
