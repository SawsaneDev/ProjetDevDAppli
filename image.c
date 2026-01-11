#include "morphing.h"
#include "uvsqgraphics_2.h"
#include <stdio.h>
#include <stdlib.h>


/*Convertit une image (jpg,png...) en format PPM en utilisant la commande 
'convert' d'ImageMagick */
void conversion_ppm(char*input, char*output){
    char cmd[512];
    /* On prepare la chaine de caractere contenant la commande système */
    snprintf(cmd, sizeof(cmd),"convert %s %s", input, output);
    /* Appel au terminal pour éxécuter la conversion*/
    system(cmd);
}

/*Lit un fichier PPM de type P3 (format texte) et remplit la structure Image */
void lire_ppm(char* filename, Image* img) {

    FILE* f = fopen(filename,"r");
    if (!f) { printf("Erreur d'ouverture %s\n", filename); return; }
    
    char format[3];
    int max_val;

    fscanf(f, "%2s", format);

    int c = fgetc(f);
    while (c == ' ' || c == '\n' || c == '\r') c = fgetc(f);
    if (c == '#') {
        while (c != '\n') c = fgetc(f);
    } else {
        ungetc(c, f);
    }

    
    /* Lecture de l'en-tete du fichier PPM */
    if (fscanf(f, "%d %d %d", &img->largeur, &img->hauteur, &max_val) != 3) {
        printf("Erreur format\n");
        fclose(f);
    }

    img->pixels = malloc(img->hauteur * sizeof(Pixel *));
    for (int i = 0; i < img->hauteur; i++){
        img->pixels[i] = malloc(img->largeur * sizeof(Pixel));
    }



    /* On parcourt chaque ligne (i) puis chaque colonne (j)*/
    for(int i=0; i<img->hauteur; i++){
        for(int j=0;j<img->largeur;j++){
            int r, g, b;

            /* Lecture des composantes Rouge, Vert, Bleu du pixel actuel*/
            if (fscanf(f,"%d %d %d",&r, &g, &b) == 3) {
                img->pixels[i][j].r= (unsigned char) r;
                img->pixels[i][j].g= (unsigned char) g;
                img->pixels[i][j].b=(unsigned char) b;
            }
        }
    }

    /* Initialisation par défaut des compteurs de points et triangles */
    img->nb_points=0;
    img->nb_triangles=0;
    fclose(f);
}

/* Affiche les images de départ et d'arrivée cote à cote dans la fenetre graphique*/
void visualiser_image(Image img1, Image img2) {
    /* La fenetre doit etre assez large pour contenir les deux images l'une à cote de l'autre*/
    init_graphics(img1.largeur + img2.largeur, img1.hauteur);

    /* Affiche de l'image de départ (à gauche) */
    for(int y=0; y < img1.hauteur; y++){
        for(int x=0; x < img1.largeur; x++){
            COULEUR c = couleur_RGB(img1.pixels[y][x].r, img1.pixels[y][x].g, img1.pixels[y][x].b);
            /* On dessine le pixel aux coordonnées (x,y)*/
            draw_pixel((POINT){x,y},c);  
        }
    }

    for(int y=0; y < img2.hauteur; y++){
        for(int x=0; x < img2.largeur; x++){
            COULEUR c= couleur_RGB(img2.pixels[y][x].r, img2.pixels[y][x].g, img2.pixels[y][x].b);
            /* On dessine le pixel aux coordonnées (x,y)*/
            draw_pixel((POINT){x + img1.largeur + 20, y}, c);  
        }
    }
    /* Pour l'image d'arrivée (à droite), il suffira d'ajouter img1.largeur à la coordonnée x*/
}

/* Enregistre les données de l'image dans un nouveau fichier PPM (format P3) */
void sauver_img_ppm(char* filename, Image* img) {
    FILE* f = fopen(filename, "w");
    if (!f) return;
    /* Ecruture de l'en tete indispensable au format PPM*/
    fprintf(f, "P3\n%d %d\n255\n", img->largeur, img->hauteur);
    
    for(int i=0; i<img->hauteur; i++) {
        for(int j=0; j<img->largeur; j++){
            /*Ecriture des triplets R G B séparés par des espaces */
            fprintf(f, "%d %d %d", img->pixels[i][j].r, img->pixels[i][j].g, img->pixels[i][j].b);
        }
        /* Saut de ligne après chaque rangée de pixels pour que le fichier soit lisible */
        fprintf(f, "\n");
    }
    fclose(f);
}
