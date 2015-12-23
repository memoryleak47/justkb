#include <linux/input.h>
#include <linux/uinput.h>

// for O_WRONLY
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// for snprintf
#include <stdio.h>

// for memset
#include <memory.h>

// for write
#include <unistd.h>

// for strerror and errno
#include <errno.h>

int main()
{
	int fd;

	fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK); // sometime it is called /dev/input/uinput
	if (fd < 0) // open(…) fails if: http://linux.die.net/man/3/open
	{
		printf("can't setup fd (%s)\n", strerror(errno));
		return 1;
	}
	if (ioctl(fd, UI_SET_EVBIT, EV_KEY) != 0)
	{
		printf("ioctl(UI_SET_EVBI, EV_KEY) throws error (%s)\n", strerror(errno));
		return 1;
	}
	if (ioctl(fd, UI_SET_EVBIT, EV_SYN) != 0)
	{
		printf("iioctl(fd, UI_SET_EVBIT, EV_SYN) throws error (%s)\n", strerror(errno));
		return 1;
	}

	// enables 'd'
	if (ioctl(fd, UI_SET_KEYBIT, KEY_D) != 0)
	{
		printf("ioctl(fd, UI_SET_KEYBIT, KEY_D) throws error (%s)\n", strerror(errno));
		return 1;
	}
	struct uinput_user_dev uidev;

	memset(&uidev, 0, sizeof(uidev));

	snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "uinput-sample");
	uidev.id.bustype = BUS_USB;
	uidev.id.vendor  = 0x1234;
	uidev.id.product = 0xfedc;
	uidev.id.version = 1;

	if (write(fd, &uidev, sizeof(uidev)) == -1) // throws error
	{
		printf("write(fd, &uidev, sizeof(uidev)) throws error (%s)\n", strerror(errno));
		return 1;
	}
	if (ioctl(fd, UI_DEV_CREATE) != 0)
	{
		printf("ioctl(fd, UI_DEV_CREATE) throws error (%s)\n", strerror(errno));
		return 1;
	}


	// press d
		struct input_event ev;

		memset(&ev, 0, sizeof(ev));

		ev.type = EV_KEY;
		ev.code = KEY_D;
		ev.value = 1;

		if (write(fd, &ev, sizeof(ev)) == -1)
		{
			printf("write(fd, &ev, sizeof(ev)) throws error (%s)\n", strerror(errno));
			return 1;
		}
	return 0;
}

