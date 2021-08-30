GENERICOBJECTS = main.o general.o memfile.o xml.o endian.o
MAINOBJECTS = brlyt.o brlan.o
OBJECTS = $(GENERICOBJECTS) $(MAINOBJECTS)
# We search within /opt/homebrew and /usr/local for x86_64 and arm64 Homebrew installs.
# /usr/local is also accessible for Cygwin.
LIBS = -L. -lmxml -lpthread -L/opt/homebrew/lib -L/usr/local/lib
INCLUDE = -I/opt/homebrew/include -I/usr/local/include
OUTPUT = benzin
all: $(OUTPUT)
%.o: %.c
	gcc -g -fno-strict-aliasing -c $(INCLUDE) -o $@ $<
$(OUTPUT): $(OBJECTS)
	gcc -g -o $(OUTPUT) $(OBJECTS) $(LIBS)
clean:
	rm -f $(OUTPUT) $(OBJECTS)
