#include "morphing.h"
#include <stdio.h>
#include <stdlib.h>

// appel systeme pour convertir les jpg/png en ppm
// on a pas reussi a faire sans la commande convert
void conversion_ppm(char* input, char* output){
    char cmd[512];
    snprintf(cmd, sizeof(cmd),"convert %s %s", input, output);
    system(cmd);
}

// fonction pour lire le ppm P3
// c'etait galere de gerer les commentaires avec les #
void lire_ppm(char* filename, Image* img) {
    FILE* f = fopen(filename,"r");
    if (!f) { printf("Erreur ouverture %s\n", filename); return; }
    
    char format[3];
    int max_val, c;

    fscanf(f, "%2s", format);
    
    // boucle pour sauter les commentaires du debut
    // sinon ca decale tout
    while (1) {
        c = fgetc(f);
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') continue;
        if (c == '#') { 
            // on lit jusqu'a la fin de la ligne
            while (c != '\n' && c != EOF) c = fgetc(f); 
            continue; 
        }
        ungetc(c, f); break;
    }

    if (fscanf(f, "%d %d %d", &img->largeur, &img->hauteur, &max_val) != 3) {
        printf("Erreur format fichier incorrect\n"); 
        fclose(f); 
        return;
    }

    // encore une boucle pour les commentaires caches avant les pixels
    // defois imagemagick en rajoute ici
    while (1) {
        c = fgetc(f);
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') continue;
        if (c == '#') { while (c != '\n' && c != EOF) c = fgetc(f); continue; }
        ungetc(c, f); break;
    }

    // allocation dynamique du tableau 2D
    img->pixels = malloc(img->hauteur * sizeof(Pixel *));
    for (int i = 0; i < img->hauteur; i++) {
        img->pixels[i] = malloc(img->largeur * sizeof(Pixel));
    }

    // remplissage des pixels
    for(int i=0; i < img->hauteur; i++){
        for(int j=0; j < img->largeur;j++) {
            int r, g, b;
            if (fscanf(f,"%d %d %d",&r, &g, &b) == 3) {
                img->pixels[i][j].r = (unsigned char)r;
                img->pixels[i][j].g = (unsigned char)g;
                img->pixels[i][j].b = (unsigned char)b;
            } else {
                // si y'a un bug on met du noir
                img->pixels[i][j].r = 0; img->pixels[i][j].g = 0; img->pixels[i][j].b = 0;
            }
        }
    }
    // init a 0
    img->nb_points=0;
    img->nb_triangles=0;
    fclose(f);
}

// sauvegarde en P3
void sauver_img_ppm(char* filename, Image* img) {
    FILE* f = fopen(filename, "w");
    if (!f) return;
    fprintf(f, "P3\n%d %d\n255\n", img->largeur, img->hauteur);
    
    for(int i=0; i<img->hauteur; i++) {
        for(int j=0; j<img->largeur; j++){
            int r = img->pixels[i][j].r;
            int g = img->pixels[i][j].g;
            int b = img->pixels[i][j].b;
            
            // fix pour ffmpeg : on clamp les valeurs
            // sinon la video plante avec des valeurs > 255
            if (r > 255) r = 255; if (r < 0) r = 0;
            if (g > 255) g = 255; if (g < 0) g = 0;
            if (b > 255) b = 255; if (b < 0) b = 0;

            fprintf(f, "%d %d %d ", r, g, b);
        }
        fprintf(f, "\n");
    }
    fclose(f);
}