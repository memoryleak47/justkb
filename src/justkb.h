#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <X11/Xlib.h>


#define die(str, args...) do { \
	perror(str); \
	exit(EXIT_FAILURE); \
	} while (0)

#define quit(str) do { \
	printf("\n%s\n", str); \
	exit(EXIT_SUCCESS); \
	} while (0)

void init();
void uninit();
void handleEvent(const XEvent &x_event);
void run(); // onEvent: calls handleEvent(XEvent)
