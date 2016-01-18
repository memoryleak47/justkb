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
	} while (0)

#define quit(str) do { \
	printf("\n%s\n", str); \
	exit(EXIT_SUCCESS); \
	} while (0)

void handleKeyEvent(unsigned int key, int state);

void init(); // calls initUinput() & initX()
void initUinput();
void initX();

void uninit(); // calls uninitUinput() & uninitX()
void uninitUinput();
void uninitX();

void moveMouse(int dx, int dy);
void applyKeyEvent(unsigned int keycode, int keyvalue);

void handleEvent(const XEvent &x_event); // calls handleKeyEvent();
void run(); // onEvent: calls handleEvent(XEvent)
