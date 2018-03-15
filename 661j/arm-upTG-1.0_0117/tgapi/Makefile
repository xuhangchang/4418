#CC = gcc
CC = arm-none-linux-gnueabihf-gcc

OTHER_HEADERS = Bmp.h md5.h 

CFLAGS += -DLINUX -D_GNU_SOURCE -Wall -shared -fPIC -L../lib -I../include -I../include/inc -Wl,-rpath ../lib/

OBJS = tgapi.o Bmp.o md5.o net.o fun.o function.o TGComApi.o 

all: libTGComApi.so

libTGComApi.so: $(OTHER_HEADERS) $(OBJS) 
	$(CC) $(CFLAGS) -o $@ $(OBJS) -lpthread -ljpeg 
	cp libTGComApi.so ../lib

clean:
	rm -f *.a *.o core *~ *.so *.lo




