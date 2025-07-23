CPP = g++
CC = gcc

CFLAGS = 
LIBS = -lcsp

CFLAGS += -Wall -Wpedantic

OBJECTS = csp_posix_helper.o
HEADERS = csp_posix_helper.h

ALL : csp_client.exe
	@echo "Listo!"

.SECONDARY: $(OBJECTS)

%.exe : %.c $(OBJECTS) $(HEADERS)
	$(CC) $< $(OBJECTS) -o $@ $(LIBS) $(CFLAGS)

%.o : %.c $(HEADERS)
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f *~ *.o *.exe
