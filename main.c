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

    init_graphics(img_dep.largeur + img_arr.largeur + 20, img_dep.hauteur);
    visualiser_image(img_dep, img_arr);

    printf("Cliquez sur les points pour le morphing.\n");
    saisir_les_points(&img_dep, &img_arr);

    sauver_points_dans_fichier("mes_points.txt", img_dep, img_arr);

    printf("Calcul du morphing.\n");
    for (int k = 0; k<= N; k++) {
        Image img_inter;

        img_inter.largeur = img_dep.largeur;
        img_inter.hauteur = img_dep.hauteur;

        calcul_image_intermediaire(img_dep, img_arr, &img_inter, k, N);
        triangulation(img_inter.points, img_inter.nb_points, img_inter.triangles, &img_inter.nb_triangles);

        char nom_temp[100];
        sprintf(nom_temp, "output/image_%d03.ppm", k);
        sauver_img_ppm(nom_temp, &img_inter);

        printf("Image %d sur %d generee\n", k, N);
    }

    printf("Creation de la video\n");
    system("ffmpeg -i output/image_%03d.ppm -r 10 mon_film.mp4");

    printf("Fin. Appuyez sur Echap pour quitter.\n");
    wait_escape();

    return 0;
}