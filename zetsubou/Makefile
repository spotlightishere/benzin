GENERICOBJECTS = main.o endian.o
MAINOBJECTS = tpl.o
OBJECTS = $(GENERICOBJECTS) $(MAINOBJECTS)
LIBS = 
OUTPUT = zetsubou
main: $(OBJECTS)
	gcc -o $(OUTPUT) $(LIBS) $(OBJECTS)
clean:
	rm -f $(OUTPUT) $(OBJECTS)
