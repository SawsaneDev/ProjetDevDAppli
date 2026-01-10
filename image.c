#include <stdio.h>
#include <stdlib.h>
#include "uvsqgraphics_2.h"
#include "../include/morphing.h"

void convert_to_ppm(char*input, char*output){

    char cmd[512];
    snprintf(cmd, sizeof(cmd),"magik convert %s %s", input, output);
    int ret=system(cmd);
    if (ret!=0){
        printf("Erreur de conversion d'image");
    }
}

Imagelire _ppm(char*filename){
    Image img;
    FILE* f= fopen(filename,"r");
    if(!f){
        printf("Impossible d'ouvrir le fichier");
        img.width=img.height=0;
        return img;
    }

    char format[3];
    fsacnf(f,"%2s\n",format);
    fscanf(f,"%d %d",&img.width,&img.height);
    int max_val;
    fscand(f,"%d\n",&max_val);

    for(int i=0;i<img.height; i++){
        for(int j=0;j<img.width;j++){
            int r,g,b;
            fscanf(f,"%d %d %d",&r,&g,&b);
            img.pixels[i][j].r= (unsigned char) r;
            img.pixels[i][j].g= (unsigned char) g;
            img.pixels[i][j].b=(unsigned char ) b;
        }
    }
    img.nb_points=0;
    img.nb_triangles=0;

    fclose(f);
    return img;
}


void afficher_images(Image img1, Image img2){
    int window.width=img1.width+img2.width;
    int window.height= (img1.height>img2.height) ? img1.height : img2.height;

    init_graphics(window_width, window_height);

    for(int y=0; y< img1.height; y++){
        for(int x=0;x<img1.width;x++){
            couleur c= couleur_RGB(img1.pixels[y][x].r,img1.pixels[y][x].g,img1.pixels[y][x].b);
            
            plot(x,y,c)   
            
        }
    }

    for(int y=0;y<img2.height;y++){
        for(int x=0;x<img2.width;x++){
            coueleur c =couleur_RGB(img2.pixels[y][x].r,img2.pixels[y][x].g,img2.pixels[y][x].b);
            plot(x+img1.width,y,c);

        }
    }

    attendre_clic(c);
    fermer_fenetre();
}
