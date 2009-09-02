GENERICOBJECTS = main.o general.o memfile.o xml.o endian.o
MAINOBJECTS = brlyt.o brlan.o
OBJECTS = $(GENERICOBJECTS) $(MAINOBJECTS)
LIBS = -L. -lmxml -lpthread
OUTPUT = benzin
all: $(OUTPUT)
%.o: %.c
	gcc -fno-strict-aliasing -c -o $@ $<
$(OUTPUT): $(OBJECTS)
	gcc  -o $(OUTPUT) $(OBJECTS) $(LIBS)
clean:
	rm -f $(OUTPUT) $(OBJECTS)
