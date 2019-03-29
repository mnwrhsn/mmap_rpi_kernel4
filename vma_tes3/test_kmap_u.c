#include<stdio.h>
#include<sys/fcntl.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<sys/mman.h>
int main(int argc,char *argv[])
{
    int fd,n,len;
    char *buff;
    if(argc != 3)
    {
        printf("Too few arguments.\n");
        exit(EXIT_FAILURE);
    }
    buff = (char *)malloc(128);
    if(strcmp(argv[1],"read")==0)
    {
        if(-1 == (fd = open("/dev/MyDevice",O_RDONLY)))
        {
            printf("Device open fail. Error: %s",strerror(errno));
            exit(EXIT_FAILURE);
        }
        memset(buff,0,128);
        if(-1 == (buff = mmap(0,128,PROT_READ,MAP_SHARED | MAP_NORESERVE,fd,0)))
        {
            printf("Mapping failed. Error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
     if(-1 == (n = read(fd,buff,128)))
        {
            printf("Device read fail. Error: %s",strerror(errno));
            exit(EXIT_FAILURE);
        }


        printf("Read from device:\n%s\n",buff);
        close(fd);

    }
    else if(strcmp(argv[1],"write")==0)
    {
        len = strlen(argv[2]);
        if(-1 == (fd = open("/dev/MyDevice",O_WRONLY)))
        {
            printf("Device open fail. Error: %s",strerror(errno));
            exit(EXIT_FAILURE);
        }

        // memset(buff,0,128);
        // if(-1 == (buff = mmap(0,128,PROT_WRITE|PROT_READ,MAP_SHARED | MAP_NORESERVE,fd,0)))
        // {
        //     printf("Mapping failed. Error: %s\n", strerror(errno));
        //     exit(EXIT_FAILURE);
        // }


        if(-1 == (n = write(fd,argv[2],len)))
        {
            printf("Device write fail. Error: %s",strerror(errno));
            exit(EXIT_FAILURE);
        }
        printf("Written %d bytes successfully.\n",n);
        close(fd);

    }
    else
    {
        printf("Invalid argument..\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}
