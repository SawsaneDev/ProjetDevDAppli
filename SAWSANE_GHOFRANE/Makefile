# Nom de l'exécutable final 
EXEC = morphing

# Compilateur et options
CC = gcc
# Les flags SDL2 sont indispensables pour trouver SDL.h 
CFLAGS = -Wall -g `sdl2-config --cflags`
# Bibliothèques : SDL2, TTF pour l'affichage et 'm' pour les calculs mathématiques 
LIBS = `sdl2-config --libs` -lSDL2_ttf -lm

# Liste des fichiers objets
OBJ = main.o image.o interface.o image_interm.o uvsqgraphics_2.o

# Règle par défaut : crée l'exécutable
all: $(EXEC)

# Lien final pour l'exécutable 
$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LIBS)

# Compilation des fichiers .c en .o
%.o: %.c morphing.h uvsqgraphics_2.h
	$(CC) -c $< $(CFLAGS)

# Nettoyage des fichiers temporaires
clean:
	rm -f $(OBJ) $(EXEC)
	rm -rf output
	@echo "Nettoyage terminé."

# Aide pour le lancement 
help:
	@echo "Usage: ./$(EXEC) Image_Depart Image_Arrivee N"