#include "justkb.h"

int ui_fd;
Display *x_display;

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
	struct uinput_user_dev ui_dev;
	ui_fd = open("/dev/uinput", O_WRONLY, O_NONBLOCK); // oder O_WRONLY | O_NDELAY

	if (ui_fd < 0)
	{
		ui_fd = open("/dev/input/uinput", O_WRONLY, O_NONBLOCK);
	}

	if (ui_fd < 0)
	{
		die("error: open");
	}

	if (ioctl(ui_fd, UI_SET_EVBIT, EV_KEY) < 0)
	{
		die("error: ioctl");
	}

	if (ioctl(ui_fd, UI_SET_KEYBIT, BTN_LEFT) < 0)
	{
		die("error: ioctl");
	}

	if (ioctl(ui_fd, UI_SET_EVBIT, EV_REL) < 0)
	{
		die("error: ioctl");
	}

	if (ioctl(ui_fd, UI_SET_RELBIT, REL_X) < 0)
	{
		die("error: ioctl");
	}

	if (ioctl(ui_fd, UI_SET_RELBIT, REL_Y) < 0)
	{
		die("error: ioctl");
	}

	for (int i = 0; i < 256; i++)
	{
		if (ioctl(ui_fd, UI_SET_KEYBIT, i) < 0)
			die("error: ioctl");
	}

	memset(&ui_dev, 0, sizeof(ui_dev));
	snprintf(ui_dev.name, UINPUT_MAX_NAME_SIZE, "uinput-sample");
	ui_dev.id.bustype = BUS_USB;
	ui_dev.id.vendor = 0x1;
	ui_dev.id.product = 0x1;
	ui_dev.id.version = 1;

	if (write(ui_fd, &ui_dev, sizeof(ui_dev)) < 0)
	{
		die("error: write");
	}

	if (ioctl(ui_fd, UI_DEV_CREATE) < 0)
	{
		die("error: ioctl");
	}

	usleep(100000);
}

void initX()
{
	if (NULL == (x_display = XOpenDisplay(NULL)))
	{
		die("x_display = XopenDisplay(NULL)");
	}

	XGrabKeyboard(x_display, DefaultRootWindow(x_display), True, GrabModeAsync, GrabModeAsync, CurrentTime);
}


void uninit()
{
	uninitUinput();
	uninitX();
}

void uninitUinput()
{
	if (ioctl(ui_fd, UI_DEV_DESTROY) < 0)
	{
		die("error: ioctl");
	}
	close(ui_fd);
}

void uninitX()
{
	XUngrabKeyboard(x_display, CurrentTime);

	if (XCloseDisplay(x_display))
	{
		die("XCloseDisplayer(x_display)");
	}
}

void moveMouse(int dx, int dy)
{
	struct input_event ui_event;
	memset(&ui_event, 0, sizeof(struct input_event));
	ui_event.type = EV_REL;

	ui_event.code = REL_X;
	ui_event.value = dx;

	if (write(ui_fd, &ui_event, sizeof(struct input_event)) < 0)
	{
		die("error: write");
	}

	memset(&ui_event, 0, sizeof(struct input_event));
	ui_event.type = EV_REL;
	ui_event.code = REL_Y;
	ui_event.value = dy;

	if (write(ui_fd, &ui_event, sizeof(struct input_event)) < 0)
	{
		die("error: write");
	}
	
	memset(&ui_event, 0, sizeof(struct input_event));
	ui_event.type = EV_SYN;
	ui_event.code = 0;
	ui_event.value = 0;

	if (write(ui_fd, &ui_event, sizeof(struct input_event)) < 0)
	{
		die("error: write");
	}
}

void applyKeyEvent(unsigned int keycode, int keyvalue) // src="http://www.linuxforums.org/forum/ubuntu-linux/161718-its-no-effect-when-using-uinput.html"
{
	struct input_event ui_event;
	gettimeofday(&ui_event.time, NULL);

	ui_event.type = EV_KEY;
	ui_event.code = keycode;
	ui_event.value = keyvalue;
	
	if (write(ui_fd, &ui_event, sizeof(ui_event)) < 0)
	{
		die("error: write");
	}

	ui_event.type = EV_SYN;
	ui_event.code = SYN_REPORT;
	ui_event.value = 0;

	if (write(ui_fd, &ui_event, sizeof(ui_event)) < 0)
	{
		die("error: write");
	}
}

void run()
{
	XEvent x_event;
	unsigned int keycode;
	int quit = 0;

	while (!quit)
	{
		XNextEvent(x_display, &x_event);
		switch (x_event.type)
		{
			case KeyPress:
				keycode = ((XKeyPressedEvent*)&x_event)->keycode;
				if (keycode == 24) // q
				{
					quit=~0;
					break;
				}
				uninitX();
				handleKeyEvent(keycode);
				initX();
				break;
			case KeyRelease:
				printf("\nTODO - release\n");
				break;
			case ButtonPress:
			case ButtonRelease:
				printf("TODO");
				break;
			case Expose:
					/* Often, it's a good idea to drain residual exposes to
					 * avoid visiting Blinky's Fun Club. */
					while (XCheckTypedEvent(x_display, Expose, &x_event)) /* empty body */ ;
				break;
			case MotionNotify:
			case ConfigureNotify:
			default:
				break;
		}
	}
}
