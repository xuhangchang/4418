#CC = gcc
CC = arm-cortexa9-linux-gnueabihf-gcc
#CFLAGS=-Wall -std=gnu99

TARGET=main.out

SRCS = main.c\
	   fun.c 

DLIBS = -lTGFingerVeinApi -lpthread

LDFLAGS =-L./lib/

RPATH = -Wl,-rpath=./lib 

OBJS = $(SRCS:.c=.o)
$(TARGET):$(OBJS) 
	$(CC) -o $@ $^ $(LDFLAGS) $(DLIBS) $(RPATH) -lm
	@echo TARGET:$@
	@echo OBJECTS:$^
#	$(CC) -o $@ $^

clean:
	rm -rf $(TARGET) $(OBJS)

exec:clean $(TARGET)
	./$(TARGET)

%.o:%.c
	$(CC) $(CFLAGS) $(INC) -o $@ -c $< -O3 
	@echo TARGET:$@
	@echo OBJECTS:$<
	@echo build:$(CC)
