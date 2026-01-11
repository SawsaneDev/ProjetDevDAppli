// Définitions des structures

#ifndef MORPHING_H
#define MORPHING_H
#include "uvsqgraphics_2.h"
#include "uvsqcouleur_2.h"

typedef struct {
    unsigned char r, g, b;
} Pixel;

typedef struct {
    double x, y;
} Point;

typedef struct{
    int p1, p2, p3;
} Triangle;

typedef struct {
    int largeur;
    int hauteur;
    Pixel **pixels;
    Point points[100];
    int nb_points;
    Triangle triangles[200];
    int nb_triangles;
} Image;

void triangulation(Point* pts, int n, Triangle* tris, int* nb_tris);

#endif

void lire_ppm(char *filename, Image *img);
void sauver_img_ppm(char* nom, Image* img);
void visualiser_image(Image img1, Image img2);
void calcul_image_intermediaire(Image dep, Image arr, Image* inter, int k, int N);
void triangulation(Point* pts, int n, Triangle* tris, int* nb_tris);
void conversion_ppm(char *input, char *output);
void saisir_les_points(Image *img_dep, Image *img_arr);
void sauver_points_dans_fichier(char *nom_fichier, Image img_dep, Image img_arr);




