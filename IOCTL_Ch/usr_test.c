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
 *	(X)Error Handling!!!!!!! //assert
 *	#preprocessor definition
 *
 *	(X)Implement Raed???
 *  (X)ssize_t read(int fd,void*buf,size_t count) and size_t fread(void*ptr,size_t size,
 *  (X)size_t nmemb,FILE*stream)
 * 	(X)fd close once should not be so many times
 *	(X)
 * 	(X)open close too much times!!
 */
#define CASE_ZERO 0
#define CASE_ONE 1
#define CASE_TWO 2
#define CASE_THREE 3


static void IOCTL_USR_TEST(int command, int fd_ioctl)
{
	int rc;
	char name[8] = "fuck";
		rc = ioctl(fd_ioctl,command,&name);
	if (rc)
	{
		printf("IOCTL is failed\n");
		printf("errno id is %d\n",errno);
		close(fd_ioctl);
		exit(0);
	}
}

int main()
{
	int fd;

	fd = open("/dev/dyn_ch_dev",O_RDWR);
	if (fd < 0)
	{
		printf("open device driver is failed\n");
		printf("errno id is %d\n",errno);
		exit(0);
	}
	IOCTL_USR_TEST(CASE_ZERO,fd);
	IOCTL_USR_TEST(CASE_ONE,fd);
	IOCTL_USR_TEST(CASE_THREE,fd);
	close(fd);
	return 0;
}


