GENERICOBJECTS = main.o general.o memfile.o xml.o endian.o
MAINOBJECTS = brlyt.o brlan.o
OBJECTS = $(GENERICOBJECTS) $(MAINOBJECTS)
LIBS = -L. -lmxml
OUTPUT = benzin
all: $(OUTPUT)
%.o: %.c
	gcc -m32 -c -o $@ $<
$(OUTPUT): $(OBJECTS)
	gcc -m32 -o $(OUTPUT) $(LIBS) $(OBJECTS)
clean:
	rm -f $(OUTPUT) $(OBJECTS)
