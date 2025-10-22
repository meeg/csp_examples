CPP = g++
CC = gcc

CFLAGS = 
LIBS = -lcsp

CFLAGS += -Wall -Wpedantic

OBJECTS = csp_posix_helper.o flatsat_lib.o
HEADERS = csp_posix_helper.h flatsat_lib.h

ALL : csp_client.exe remotecli.exe
	@echo "Listo!"

.SECONDARY: $(OBJECTS)

%.exe : %.c $(OBJECTS) $(HEADERS)
	$(CC) $< $(OBJECTS) -o $@ $(LIBS) $(CFLAGS)

%.o : %.c $(HEADERS)
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f *~ *.o *.exe
