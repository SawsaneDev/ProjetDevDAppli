// Définitions des structures

#ifndef MORPHING_H
#define MORPHING_H
#include <uvsqgraphics.h>

typedef struct {
    unsigned char r, v, b;
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
    Pixel pixels[800][800];
    Point points[100];
    int nb_points;
    Triangle triangles[200];
    int nb_triangles;
} Image;

#endif

void lire_ppm(char *nom_fichier, Image *img);
void calculer_points_inter(Point *dep, Point *arr, Point *res, float alpha);


