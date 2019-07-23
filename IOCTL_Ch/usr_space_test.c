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


int main()
{
	int fd, rc;
	char name[8] = "fuck";

	fd = open("/dev/mychardev", O_RDWR);
	if (fd < 0)
	{
		printf("the open is failed\n");
		printf("errno id is %d\n",errno);
		close(fd);
		exit(0);
	}

	//assert(fd > 0);//google `assert`

	rc = ioctl(fd,0, &name);
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


