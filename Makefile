# Nom de l'exécutable
EXEC = morphing

# Fichiers sources
SRC = main.c interface.c image.c image_interm.c uvsqgraphics_2.c

# Options de compilation
CFLAGS = -Wall -Wextra
LIBS = -lSDL -lSDL_image -lm

# Compilation
all:
	gcc $(CFLAGS) $(SRC) -o $(EXEC) $(LIBS)

# Nettoyer les fichiers compilés
clean:
	rm -f $(EXEC) *.o

	
