#include "justkb.h"

void handleKeyEvent(unsigned int key)
{
	printf("key is: %d\n", key);
	applyKeyEvent(key, 1);
	applyKeyEvent(key, 0);
}

void init()
{
	initUinput();
	initX();
}

void initUinput()
{
	struct uinput_user_dev uidev;
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

void initX()
{
	if (NULL==(dpy=XOpenDisplay(NULL)))
	{
		die("dpy=XopenDisplay(NULL)");
	}

	/*
	 * You might want to warp the pointer to somewhere that you know
	 * is not associated with anything that will drain events.
	 *  (void)XWarpPointer(dpy, None, DefaultRootWindow(dpy), 0, 0, 0, 0, x, y);
	 */

	XGrabKeyboard(dpy, DefaultRootWindow(dpy), True, GrabModeAsync, GrabModeAsync, CurrentTime);
}


void uninit()
{
	uninitUinput();
	uninitX();
}

void uninitUinput()
{
	if (ioctl(fd, UI_DEV_DESTROY) < 0)
		die("error: ioctl");
	close(fd);

}

void uninitX()
{
	XUngrabKeyboard(dpy, CurrentTime);

	if (XCloseDisplay(dpy))
	{
		die("XCloseDisplayer(dpy)");
	}
}

void moveMouse(int dx, int dy)
{
	struct input_event iev;
	memset(&iev, 0, sizeof(struct input_event));
	iev.type = EV_REL;

	iev.code = REL_X;
	iev.value = dx;
	if (write(fd, &iev, sizeof(struct input_event)) < 0)
		die("error: write");

	memset(&iev, 0, sizeof(struct input_event));
	iev.type = EV_REL;
	iev.code = REL_Y;
	iev.value = dy;
	if (write(fd, &iev, sizeof(struct input_event)) < 0)
		die("error: write");
	
	memset(&iev, 0, sizeof(struct input_event));
	iev.type = EV_SYN;
	iev.code = 0;
	iev.value = 0;
	if (write(fd, &iev, sizeof(struct input_event)) < 0)
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

void run()
{
	XEvent xev;
	unsigned int kc;
	int quit = 0;

	while (!quit)
	{
		XNextEvent(dpy, &xev);
		switch (xev.type)
		{
			case KeyPress:
				kc = ((XKeyPressedEvent*)&xev)->keycode;
				if (kc == 24) // kc is q
				{
					quit=~0;
					break;
				}
				uninitX();
				handleKeyEvent(kc);
				initX();
				break;
			case Expose:
					/* Often, it's a good idea to drain residual exposes to
					 * avoid visiting Blinky's Fun Club. */
					while (XCheckTypedEvent(dpy, Expose, &xev)) /* empty body */ ;
				break;
			case ButtonPress:
			case ButtonRelease:
			case KeyRelease:
			case MotionNotify:
			case ConfigureNotify:
			default:
				break;
		}
	}
}
