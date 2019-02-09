CC = gcc
CCFLAGS = -g
TARGET = io_event_server

#Collect all .c source codes
S_TEMP = $(wildcard ./*.c)

#Filter out any .c files that should not be part of this executable
S_FILTER = ./socket_client_inet.c ./io_event1.c
SOURCE = $(filter-out $(S_FILTER),$(S_TEMP))

#Collect all header libraries
INCLUDE = -I.

all:
	$(CC) $(SOURCE) $(INCLUDE) $(CCFLAGS) -o $(TARGET)
