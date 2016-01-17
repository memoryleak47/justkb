#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>

#include <X11/Xlib.h>


#define die(str, args...) do { \
	perror(str); \
	exit(EXIT_FAILURE); \
	} while(0)

int fd;
struct uinput_user_dev uidev;
struct input_event iev;

Display *dpy;
XEvent xev;
char *s;

void init();
void initUinput();
void initX();

void uninit();
void uninitUinput();
void uninitX();

void moveMouse(int dx, int dy);
void applyKeyEvent(unsigned int keycode, int keyvalue);
void run();
