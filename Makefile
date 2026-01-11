# Makefile final pour le projet Morphing

CC = gcc

# C'est ICI que la magie opère : on demande à sdl2-config où sont les fichiers .h
# Cela corrige l'erreur "fatal error: SDL.h: No such file"
CFLAGS = -Wall -g `sdl2-config --cflags`

# Les bibliothèques pour l'étape finale (le collage)
LIBS = `sdl2-config --libs` -lSDL2_ttf -lm

# Nom de ton exécutable
TARGET = morphing

# Liste de tous les fichiers objets nécessaires
OBJ = main.o image.o interface.o image_interm.o uvsqgraphics_2.o

all: $(TARGET)

# Etape finale : on assemble tous les morceaux
$(TARGET): $(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(LIBS)

# Compilation de chaque fichier .c en .o
# Grâce à $(CFLAGS), il saura où trouver uvsqgraphics.h et SDL.h !

main.o: main.c morphing.h uvsqgraphics_2.h
	$(CC) -c main.c $(CFLAGS)

image.o: image.c morphing.h uvsqgraphics_2.h
	$(CC) -c image.c $(CFLAGS)

interface.o: interface.c morphing.h uvsqgraphics_2.h
	$(CC) -c interface.c $(CFLAGS)

image_interm.o: image_interm.c morphing.h uvsqgraphics_2.h
	$(CC) -c image_interm.c $(CFLAGS)

uvsqgraphics_2.o: uvsqgraphics_2.c uvsqgraphics_2.h
	$(CC) -c uvsqgraphics_2.c $(CFLAGS)

clean:
	rm -f *.o $(TARGET)