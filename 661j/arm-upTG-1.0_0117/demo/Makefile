#CC = gcc
CC = arm-none-linux-gnueabihf-gcc
#CFLAGS=-Wall -std=gnu99

TARGET=main.out

SRCS = main.c\
	   fun.c \

INC =

#DLIBS =-L../lib/ -lTGComApi -lTGFingerVeinApi

#LDFLAGS =-L../lib/
LDFLAGS =-L../lib/

#RPATH = -Wl,-rpath=./lib 
RPATH = -Wl,-rpath=../lib 

OBJS = $(SRCS:.c=.o)
$(TARGET):$(OBJS) ../lib/libTGComApi.so ../lib/libTGFingerVeinApi.so
	make -C ../tgapi all
#	$(CC) -o $@ $^ $(LDFLAGS) $(DLIBS) -lm -lpthread
	$(CC) -o $@ $^ $(LDFLAGS) $(DLIBS) $(RPATH) -lm -lpthread -ljpeg
	@echo TARGET:$@
	@echo OBJECTS:$^
#	$(CC) -o $@ $^

clean:
	make -C ../tgapi $@
	rm -rf $(TARGET) $(OBJS)

exec:clean $(TARGET)
	./$(TARGET)

%.o:%.c
	$(CC) $(CFLAGS) $(INC) -o $@ -c $< -O3
	@echo TARGET:$@
	@echo OBJECTS:$<
	@echo build:$(CC)
