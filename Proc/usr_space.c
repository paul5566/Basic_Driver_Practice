#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(void)
{
	char buf[128] = "Hey Dude";
	int fd = open("/proc/read_hndlr", O_RDWR);

	read(fd, buf, sizeof(buf));
	puts(buf);
	lseek(fd, 0 , SEEK_SET);
	write(fd, buf, 5);
	lseek(fd, 0 , SEEK_SET);
	read(fd, buf, sizeof(buf));
	puts(buf);

	return 0;
}
