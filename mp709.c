#include <linux/types.h>
#include <linux/input.h>
#include <linux/hidraw.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

typedef int bool;
#define true 1
#define false 0

int main(int argc, char **argv) {
	int fd, res;
	char buf[256];
	char dev_name[32];
	char dev_num_str[10];
	bool has_device = false;
	struct stat st;

	printf("debug argc: %d\n", argc);

	if (2 == argc || argc > 3) {
		printf("usage:\n");
		printf("%s - get list of devices\n", argv[0]);
		printf("%s on|off <device_number> - set up device status\n", argv[0]);
		exit(1);
	}


	int i;
	for (i=0; i<9; ++i) {
		sprintf(dev_name, "/dev/hidraw%i", i);

		if (stat(dev_name, &st) < 0)
			break;

		fd = open(dev_name, O_RDWR|O_NONBLOCK);

		if (fd < 0) {
			perror("Unable to open device");
			continue;
		};

		res = ioctl(fd, HIDIOCGRAWNAME(256), buf);
		if (res < 0)
			perror("Cannot get device name");
		else
			printf("/dev/hidraw%i: %s\n", i, buf);

		if (strstr(buf, "MP709") != NULL) {
			has_device = true;

			sprintf(dev_num_str, "%d", i);
			if (argv[2][0] == dev_num_str[0]) {
				break;
			}
		}
		close(fd);
	};

	if (!has_device) {
		printf("No device found\n");
		return 1;
	};

	if (argc < 3) {
		return 0;
	}

	buf[0] = 0xE7;

	if (strcasecmp(argv[1], "on") == 0)
		buf[1] = 0x0;
	else
		buf[1] = 0x19;

	printf("turning %s device %s\n", argv[1], dev_name);

	res = write(fd, buf, 2);
	if (res < 0) {
		printf("Error: %d\n", errno);
		perror("write");
	};

	close(fd);

	return 0;
}
