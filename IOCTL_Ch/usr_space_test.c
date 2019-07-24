#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <asm/types.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
/*
 * This is the user space program when we excuted it
 * the kernel log message printf the "fuck"
 */
/*
 *	Error Handling!!!!!!! //assert
 *	#preprocessor definition
 *
 *	#define IO_BUFFER 0
 *	#define IO_READ 1
 *
 *
 *	Implement Raed???
 * ssize_t read(int fd,void*buf,size_t count) and size_t fread(void*ptr,size_t size,
 * size_t nmemb,FILE*stream)
 */

#define GET_USR 0
#define PUT_USR 1
#define COPY_FROM_USR 2
#define COPY_TO_USR 3


//
int main()
{
	int fd, rc;
	char name[8] = "fuck";
	char buffer_kernel[16];
	fd = open("/dev/mychardev", O_RDWR);
	if (fd < 0)
	{
		printf("the open is failed\n");
		printf("errno id is %d\n",errno);
		exit(0);
	}

	//assert(fd > 0);//google `assert`

	rc = ioctl(fd,2,&name);
	//assert(fd > 0);
	if (rc < 0)
	{
		printf("the open is failed\n");
		printf("errno id is %d\n",errno);
		close(fd);
		exit(0);
	}
	close(fd);
	return 0;
}


