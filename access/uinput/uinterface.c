#include "uinterface.h"

void init()
{
	fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
	if (fd < 0)
		die("error: open");

	if (ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0)
		die("error: ioctl");
	if (ioctl(fd, UI_SET_KEYBIT, BTN_LEFT) < 0)
		die("error: ioctl");

	if (ioctl(fd, UI_SET_EVBIT, EV_REL) < 0)
		die("error: ioctl");
	if (ioctl(fd, UI_SET_RELBIT, REL_X) < 0)
		die("error: ioctl");
	if (ioctl(fd, UI_SET_RELBIT, REL_Y) < 0)
		die("error: ioctl");

	memset(&uidev, 0, sizeof(uidev));
	snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "uinput-sample");
	uidev.id.bustype = BUS_USB;
	uidev.id.vendor = 0x1;
	uidev.id.product = 0x1;
	uidev.id.version = 1;

	if (write(fd, &uidev, sizeof(uidev)) < 0)
		die("error: write");

	if (ioctl(fd, UI_DEV_CREATE) < 0)
		die("error: ioctl");
	sleep(2);
}

void uninit()
{
	if (ioctl(fd, UI_DEV_DESTROY) < 0)
		die("error: ioctl");
	close(fd);
}

void moveMouse(int dx, int dy)
{
	int i;
	for (i = 0; i < 20; i++)
	{
		memset(&ev, 0, sizeof(struct input_event));
		ev.type = EV_REL;
		ev.code = REL_X;
		ev.value = dx;
		if(write(fd, &ev, sizeof(struct input_event)) < 0)
			die("error: write");

		memset(&ev, 0, sizeof(struct input_event));
		ev.type = EV_REL;
		ev.code = REL_Y;
		ev.value = dy;
		if(write(fd, &ev, sizeof(struct input_event)) < 0)
			die("error: write");
		
		memset(&ev, 0, sizeof(struct input_event));
		ev.type = EV_SYN;
		ev.code = 0;
		ev.value = 0;
		if (write(fd, &ev, sizeof(struct input_event)) < 0)
			die("error: write");
	}
	usleep(15000);
}
