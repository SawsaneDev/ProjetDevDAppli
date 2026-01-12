#include "morphing.h"
#include "uvsqgraphics_2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int est_ppm(char *nom_fichier) {
    int longueur = strlen(nom_fichier);
    if (longueur > 4 && strcmp(nom_fichier + longueur - 4, ".ppm") ==0) {
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[]) {

    system("mkdir -p output");

    if (argc < 4) {
        printf("Usage: %s image1 image2 nb_images\n", argv[0]);
        return 1;
    }

    char *img_source_1 = argv[1];
    char *img_source_2 = argv[2];
    int N = atoi(argv[3]);

    char *fichier_final_1 = img_source_1;
    char *fichier_final_2 = img_source_2;

    if (!est_ppm(img_source_1)) {
        printf("Conversion de Image 1 au format PPM.\n");
        conversion_ppm(img_source_1, "temp1.ppm");
        fichier_final_1 = "temp1.ppm";
    }

    if (!est_ppm(img_source_2)) {
        printf("Conversion de Image 2 au format PPM.\n");
        conversion_ppm(img_source_2, "temp2.ppm");
        fichier_final_2 = "temp2.ppm";
    }
        
    Image img_dep, img_arr;

    lire_ppm(fichier_final_1, &img_dep);
    lire_ppm(fichier_final_2, &img_arr);


    visualiser_image(img_dep, img_arr);

    printf("Cliquez sur les points pour le morphing.\n");
    saisir_les_points(&img_dep, &img_arr);

    sauver_points_dans_fichier("mes_points.txt", img_dep, img_arr);

    printf("Calcul du morphing.\n");

    for (int i = img_dep.nb_points - 1; i >= 0; i--) {
        img_dep.points[i + 4] = img_dep.points[i];
        img_arr.points[i + 4] = img_arr.points[i];
    }
    
    img_dep.points[0] = (Point){0, 0};
    img_dep.points[1] = (Point){img_dep.largeur - 1, 0};
    img_dep.points[2] = (Point){img_dep.largeur - 1, img_dep.hauteur - 1};
    img_dep.points[3] = (Point){0, img_dep.hauteur - 1};

    img_arr.points[0] = (Point){0, 0};
    img_arr.points[1] = (Point){img_arr.largeur - 1, 0};
    img_arr.points[2] = (Point){img_arr.largeur - 1, img_arr.hauteur - 1};
    img_arr.points[3] = (Point){0, img_arr.hauteur - 1};

    img_dep.nb_points += 4;
    img_arr.nb_points += 4;


    system ("mkdir -p output");
    
    for (int k = 0; k<= N; k++) {
        Image img_inter;
        img_inter.largeur = img_dep.largeur;
        img_inter.hauteur = img_dep.hauteur;

        img_inter.pixels = malloc(img_inter.hauteur * sizeof(Pixel *));
        for (int i = 0; i < img_inter.hauteur; i++) {
            img_inter.pixels[i] = malloc(img_inter.largeur * sizeof(Pixel));
        }

        calcul_image_intermediaire(img_dep, img_arr, &img_inter, k, N);
       
        char nom_temp[100];
        sprintf(nom_temp, "output/image_%03d.ppm", k);
        sauver_img_ppm(nom_temp, &img_inter);

        printf("Image %d sur %d generee\n", k, N);
    
        for (int i = 0; i<img_inter.hauteur; i++) {
            free(img_inter.pixels[i]);
        }
        free(img_inter.pixels);
    }
// 

    printf("Creation de la video\n");
    system("ffmpeg -y -framerate 25 -i output/image_%03d.ppm -vf \"scale=trunc(iw/2)*2:trunc(ih/2)*2\" -c:v libx264 -pix_fmt yuv420p mon_film.mp4");
    printf("Fin. Appuyez sur Echap pour quitter.\n");
    wait_escape();

for (int i = 0; i < img_dep.hauteur; i++) {
        free(img_dep.pixels[i]);
}
    free(img_dep.pixels);

for (int i = 0; i < img_arr.hauteur; i++) {
        free(img_arr.pixels[i]);
}
    free(img_arr.pixels);

    return 0;
}



