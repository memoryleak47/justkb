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

int main()
{
	int fd;

	fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK); // sometime it is called /dev/input/uinput
	if (fd < 0) // open(…) fails if: http://linux.die.net/man/3/open
	{
		printf("can't setup fd");
		return 1;
	}
	/*ret = */ioctl(fd, UI_SET_EVBIT, EV_KEY);
	/*ret = */ioctl(fd, UI_SET_EVBIT, EV_SYN);

	// enabled 'd'
	/*ret = */ioctl(fd, UI_SET_KEYBIT, KEY_D);

	struct uinput_user_dev uidev;

	memset(&uidev, 0, sizeof(uidev));

	snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "uinput-sample");
	uidev.id.bustype = BUS_USB;
	uidev.id.vendor  = 0x1234;
	uidev.id.product = 0xfedc;
	uidev.id.version = 1;

	/*ret = */write(fd, &uidev, sizeof(uidev));
	/*ret = */ioctl(fd, UI_DEV_CREATE);


	// press d
		struct input_event ev;

		memset(&ev, 0, sizeof(ev));

		ev.type = EV_KEY;
		ev.code = KEY_D;
		ev.value = 1;

		/*ret = */write(fd, &ev, sizeof(ev));
	return 0;
}

