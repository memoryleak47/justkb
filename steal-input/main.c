#include <X11/Xlib.h>

int main(int argc, char **argv)
{
	Display *dpy;
	XEvent ev;
	char *s;
	unsigned int kc;
	int quit = 0;

	if (NULL==(dpy=XOpenDisplay(NULL))) {
		perror(argv[0]);
		exit(1);
	}

	/*
	 * You might want to warp the pointer to somewhere that you know
	 * is not associated with anything that will drain events.
	 *  (void)XWarpPointer(dpy, None, DefaultRootWindow(dpy), 0, 0, 0, 0, x, y);
	 */

	XGrabKeyboard(dpy, DefaultRootWindow(dpy),
					  True, GrabModeAsync, GrabModeAsync, CurrentTime);

	printf("KEYBOARD GRABBED!  Hit 'q' to quit!\n"
			 "If this job is killed or you get stuck, use Ctrl-Alt-F1\n"
			 "to switch to a console (if possible) and run something that\n"
			 "ungrabs the keyboard.\n");


	/* A very simple event loop: start at "man XEvent" for more info. */
	/* Also see "apropos XGrab" for various ways to lock down access to
	 * certain types of info. coming out of or going into the server */
	for (;!quit;)
	{
		XNextEvent(dpy, &ev);
		switch (ev.type)
		{
			case KeyPress:
				kc = ((XKeyPressedEvent*)&ev)->keycode;
				s = XKeysymToString(XKeycodeToKeysym(dpy, kc, 0));
				/* s is NULL or a static no-touchy return string. */
				if (s) printf("KEY:%s\n", s);
				if (!strcmp(s, "q")) quit=~0;
				break;
			case Expose:
					/* Often, it's a good idea to drain residual exposes to
					 * avoid visiting Blinky's Fun Club. */
					while (XCheckTypedEvent(dpy, Expose, &ev)) /* empty body */ ;
				break;
			case ButtonPress:
			case ButtonRelease:
			case KeyRelease:
			case MotionNotify:
			case ConfigureNotify:
			default:
				printf("something happend\n");
				break;
		}
	}

	XUngrabKeyboard(dpy, CurrentTime);

	if (XCloseDisplay(dpy))
	{
		perror(argv[0]);
		exit(1);
	}

	return 0;
}
