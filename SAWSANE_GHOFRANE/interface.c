#include "uvsqgraphics_2.h"
#include "morphing.h"

/*void saisir_les_points(Image *img_dep, Image *img_arr) {
    int i;
    printf("Début de la saisie des points\n");
    printf("Ciquez sur l'image de gauche puis sur l'image de droite\n");

    img_dep->points[0] = (Point){0, 0}; img_arr->points[0] = (Point){0, 0};
    img_dep->points[1] = (Point){img_dep->largeur-1, 0}; img_arr->points[1] = (Point){img_arr->largeur-1, 0};
    img_dep->points[2] = (Point){img_dep->largeur-1, img_dep->hauteur-1}; img_arr->points[2] = (Point){img_arr->largeur-1, img_arr->hauteur-1};
    img_dep->points[3] = (Point){0, img_dep->hauteur-1}; img_arr->points[3] = (Point){img_arr->hauteur-1};

    img_dep->nb_points = 4;
    img_arr->nb_points = 4;

    i = 4;
    while (i < 100) {
        POINT p1 = wait_clic();

        if (p1.y > img_dep->hauteur - 50) break;

        printf("Point %d : Cliquez sur l'image de Gauche\n", i);
        draw_circle(p1, 5, rouge);
        aff_int(i, 10, p1, rouge);

        img_dep->points[i].x = p1.x;
        img_dep->points[i].y = p1.y;

        printf("Point %d : Cliquez sur l'image de Droite\n", i);
        POINT p2 = wait_clic();
        draw_circle(p2, 5, bleu);
        aff_int(i, 10, p2, bleu);

        img_arr->points[i].x = p2.x;
        img_arr->points[i].y = p2.y;

        img_dep->nb_points++;
        img_arr->nb_points++;

        printf("Les points %d sont bien enregistres.\n", i);
    }
}*/

void sauver_points_dans_fichier(char *nom_fichier, Image img_dep, Image img_arr) {
    FILE *f = fopen(nom_fichier, "w");
    if (f == NULL) {
        printf("Erreur : ouverture du fichier impossible.\n");
        return;
    }

    for (int j = 0; j < img_dep.nb_points; j++) {
        fprintf(f, "%f %f %f %f\n",
            img_dep.points[j].x, img_dep.points[j].y,
            img_arr.points[j].x, img_arr.points[j].y);
    }
    fclose(f);
    printf("Les point sont sauvegardes dans %s .\n", nom_fichier);
}




