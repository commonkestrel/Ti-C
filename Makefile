NAME = PONG
ICON = icon.png
DESCRIPTION = "Just a test, first time in C"
COMPRESSED = YES
HAS_PRINTF = NO


CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

include $(shell cedev-config --makefile)