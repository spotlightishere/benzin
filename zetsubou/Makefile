GENERICOBJECTS = main.o endian.o
MAINOBJECTS = tpl.o
OBJECTS = $(GENERICOBJECTS) $(MAINOBJECTS)
LIBS = -lgd
OUTPUT = zetsubou
main: $(OBJECTS)
	gcc -o $(OUTPUT) $(LIBS) $(OBJECTS)
clean:
	rm -f $(OUTPUT) $(OBJECTS)
