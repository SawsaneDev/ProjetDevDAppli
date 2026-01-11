#include "morphing.h"
#include "uvsqgraphics_2.h"
#include <stdio.h>
#include <stdlib.h>

void conversion_ppm(char*input, char*output){
    char cmd[512];
    snprintf(cmd, sizeof(cmd),"convert %s %s", input, output);
    system(cmd);
}

void lire_ppm(char* filename, Image* img) {
    FILE* f = fopen(filename,"r");
    if(!f) {
        printf("Erreur.\n", filename);
        return;
    }

    char format[3];
    fscanf(f, "%2s", format);


    if (fscanf(f, "%d %d", &img->largeur, &img->hauteur) != 2) {
        printf("Erreur de lecture des dimensions dans %s\n", filename);
        fclose(f);
        return;
    }
    
    int max_val;
    fscanf(f, "%d", &max_val);


    for(int i=0; i<img->hauteur; i++){
        for(int j=0;j<img->largeur;j++){
            int r, g, b;
            if (fscanf(f,"%d %d %d",&r, &g, &b));
            img->pixels[i][j].r= (unsigned char) r;
            img->pixels[i][j].g= (unsigned char) g;
            img->pixels[i][j].b=(unsigned char ) b;
        }
    }
    img->nb_points=0;
    img->nb_triangles=0;
    fclose(f);
}


void visualiser_image(Image img1, Image img2) {
    init_graphics(img1.largeur + img2.largeur, img1.hauteur);

    for(int y=0; y< img1.hauteur; y++){
        for(int x=0; x<img1.largeur;x++){
            COULEUR c= couleur_RGB(img1.pixels[y][x].r,img1.pixels[y][x].g,img1.pixels[y][x].b);
            draw_pixel((POINT){x,y},c);  
        }
    }
}

void sauver_img_ppm(char* filename, Image* img) {
    FILE* f = fopen(filename, "w");
    fprintf(f, "P3_\n%d %d\n255\n", img->largeur, img->hauteur);
    for(int i=0; i<img->hauteur; i++) {
        for(int j=0; j<img->largeur; j++){
            fprintf(f, "%d %d %d", img->pixels[i][j].r, img->pixels[i][j].g, img->pixels[i][j].b);
        }
        fprintf(f, "\n");
    }
    fclose(f);
    wait_clic();
    wait_escape();
}
