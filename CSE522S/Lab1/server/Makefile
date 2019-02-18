
CC = gcc
CCFLAGS = -g
TARGET = server_lab1

#Collect all .c source codes
S_TEMP = $(wildcard ./*.c) $(wildcard ../*.c)

#Filter out any .c files that should not be part of this executable
S_FILTER =  
SOURCE = $(filter-out $(S_FILTER),$(S_TEMP))

#Collect all header libraries
INCLUDE = -I. -I../

all:
	$(CC) $(SOURCE) $(INCLUDE) $(CCFLAGS) -o $(TARGET)