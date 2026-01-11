all:
	gcc main.c interface.c image.c image_interm.c uvsqgraphics_2.c \
	-o morphing \
	-lSDL -lSDL_image -lm
	
