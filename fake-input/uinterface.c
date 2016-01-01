#include "uinterface.h"

void init()
{
	fd = open("/dev/uinput", O_WRONLY, O_NONBLOCK); // oder O_WRONLY | O_NDELAY
	if (fd < 0)
		fd = open("/dev/input/uinput", O_WRONLY, O_NONBLOCK);
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
	int i = 0;
	for (i = 0; i < 256; i++)
	{
		if (ioctl(fd, UI_SET_KEYBIT, i) < 0)
			die("error: ioctl");
	}

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
	usleep(100000);
}

void uninit()
{
	if (ioctl(fd, UI_DEV_DESTROY) < 0)
		die("error: ioctl");
	close(fd);
}

void moveMouse(int dx, int dy)
{
	memset(&ev, 0, sizeof(struct input_event));
	ev.type = EV_REL;
	ev.code = REL_X;
	ev.value = dx;
	if (write(fd, &ev, sizeof(struct input_event)) < 0)
		die("error: write");

	memset(&ev, 0, sizeof(struct input_event));
	ev.type = EV_REL;
	ev.code = REL_Y;
	ev.value = dy;
	if (write(fd, &ev, sizeof(struct input_event)) < 0)
		die("error: write");
	
	memset(&ev, 0, sizeof(struct input_event));
	ev.type = EV_SYN;
	ev.code = 0;
	ev.value = 0;
	if (write(fd, &ev, sizeof(struct input_event)) < 0)
		die("error: write");
}

void applyKeyEvent(unsigned int keycode, int keyvalue) // src="http://www.linuxforums.org/forum/ubuntu-linux/161718-its-no-effect-when-using-uinput.html"
{
	struct input_event event;
	gettimeofday(&event.time, NULL);

	event.type = EV_KEY;
	event.code = keycode;
	event.value = keyvalue;
	
	if (write(fd, &event, sizeof(event)) < 0)
	{
		die("error: write");
	}

	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;

	if (write(fd, &event, sizeof(event)) < 0)
	{
		die("error: write");
	}
}
