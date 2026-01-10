#include "morphing.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    init_graphics(1200, 600);
    printf("Test du projet Morphing...\n")

    draw_fill_rect((POINT){0,0}, (PONIT){1200, 600}, blanc);
    draw_printf((POINT){450, 300}, noir, 20, "Projet Morphing Pret");

    draw_line((POINT){600,0}, (POINT){600, 600}, rouge);
    draw_prontf((POINT){200, 550}, bleu, 15, "Image de depart");
    draw_prontf((POINT){800, 550}, bleu, 15, "Image de depart");

    prontf("Cliquez dans la fenetre pour quittez.\n");
    wait_escape();

    return 0;
}