#include "uvsqgraphics_2.h"
#include "morphing.h"
#include <stdio.h>
#include <stdlib.h>

// fonction produit vectoriel pour savoir si P est dans ABC
// trouve sur internet la formule
int point_dans_triangle(Point p, Point a, Point b, Point c) {
    float d1 = (p.x - b.x) * (a.y - b.y) - (a.x - b.x) * (p.y - b.y);
    float d2 = (p.x - c.x) * (b.y - c.y) - (b.x - c.x) * (p.y - c.y);
    float d3 = (p.x - a.x) * (c.y - a.y) - (c.x - a.x) * (p.y - a.y);
    // faut que tous les signes soient pareils
    return ((d1 >= 0 && d2 >= 0 && d3 >= 0) || (d1 <= 0 && d2 <= 0 && d3 <= 0));
}

// calcul des coefs barycentriques (lambda mu etc)
// voir le pdf du projet section 4
void calcul_barycentrique(Point P, Point A, Point B, Point C, float* l1, float* l2, float* l3) {
    float detT = (B.y - C.y) * (A.x - C.x) + (C.x - B.x) * (A.y - C.y);
    // protection division par zero
    if (detT == 0) { *l1 = -1; return; } 
    
    *l1 = ((B.y - C.y) * (P.x - C.x) + (C.x - B.x) * (P.y - C.y)) / detT;
    *l2 = ((C.y - A.y) * (P.x - C.x) + (A.x - C.x) * (P.y - C.y)) / detT;
    *l3 = 1.0f - *l1 - *l2; 
}

// interpolation couleur simple
Pixel interpolation_pixel(Pixel d, Pixel a, float alpha) {
    Pixel p;
    // cast en unsigned char obligatoire sinon warning
    p.r = (unsigned char)((1 - alpha) * d.r + alpha * a.r);
    p.g = (unsigned char)((1 - alpha) * d.g + alpha * a.g);
    p.b = (unsigned char)((1 - alpha) * d.b + alpha * a.b);
    return p;
}

// calcule la position des points pour l'image intermediaire
void calcul_points_intermediaires(Point* dep, Point* arr, Point* inter, int n, float alpha) {
    for (int i = 0; i < n; i++) {
        inter[i].x = (1 - alpha) * dep[i].x + alpha * arr[i].x;
        inter[i].y = (1 - alpha) * dep[i].y + alpha * arr[i].y;
    }
}

// algorithme de triangulation
// on part des 4 coins et on decoupe les triangles
void triangulation(Point* pts, int n, Triangle* tris, int* nb_tris) {
    if (n < 5) { *nb_tris = 0; return; }
    
    // init avec 4 triangles
    tris[0] = (Triangle){0, 1, 4};
    tris[1] = (Triangle){1, 2, 4};
    tris[2] = (Triangle){2, 3, 4};
    tris[3] = (Triangle){3, 0, 4};
    *nb_tris = 4;
    
    // on ajoute les points un par un
    for (int p = 5; p < n; p++) {
        for (int t = 0; t < *nb_tris; t++) {
            Triangle tr = tris[t];
            // si le point est dedans on split en 3
            if (point_dans_triangle(pts[p], pts[tr.p1], pts[tr.p2], pts[tr.p3])) {
                tris[t] = tris[--(*nb_tris)]; // suppr l'ancien
                tris[(*nb_tris)++] = (Triangle){tr.p1, tr.p2, p};
                tris[(*nb_tris)++] = (Triangle){tr.p2, tr.p3, p};
                tris[(*nb_tris)++] = (Triangle){tr.p3, tr.p1, p};
                break; 
            }       
        }
    }
}

// fonction principale qui genere l'image complete
// c'est lourd en calculs !
void calcul_image_intermediaire(Image dep, Image arr, Image* inter, int k, int N) {
    float alpha = (float)k / N; 
    inter->largeur = dep.largeur;
    inter->hauteur = dep.hauteur;
    inter->nb_points = dep.nb_points;

    calcul_points_intermediaires(dep.points, arr.points, inter->points, dep.nb_points, alpha);
    triangulation(inter->points, inter->nb_points, inter->triangles, &inter->nb_triangles);

    // double boucle sur tous les pixels (y puis x)
    for (int y = 0; y < dep.hauteur; y++) {
        for (int x = 0; x < dep.largeur; x++) {
            Point P = {(float)x, (float)y};
            int trouve = 0;
            
            // on cherche le triangle qui contient le pixel
            for (int t = 0; t < inter->nb_triangles; t++) {
                Triangle tr = inter->triangles[t];
                Point A = inter->points[tr.p1];
                Point B = inter->points[tr.p2];
                Point C = inter->points[tr.p3];
                float l1, l2, l3;
                
                calcul_barycentrique(P, A, B, C, &l1, &l2, &l3);

                // si coord positives, c'est dedans
                if (l1 >= 0 && l2 >= 0 && l3 >= 0) {
                    float xD = l1 * dep.points[tr.p1].x + l2 * dep.points[tr.p2].x + l3 * dep.points[tr.p3].x;
                    float yD = l1 * dep.points[tr.p1].y + l2 * dep.points[tr.p2].y + l3 * dep.points[tr.p3].y;
                    float xA = l1 * arr.points[tr.p1].x + l2 * arr.points[tr.p2].x + l3 * arr.points[tr.p3].x;
                    float yA = l1 * arr.points[tr.p1].y + l2 * arr.points[tr.p2].y + l3 * arr.points[tr.p3].y;
                    
                    // securite pour pas sortir du tableau (segfault)
                    int ixD = (int)xD; if(ixD<0) ixD=0; if(ixD>=dep.largeur) ixD=dep.largeur-1;
                    int iyD = (int)yD; if(iyD<0) iyD=0; if(iyD>=dep.hauteur) iyD=dep.hauteur-1;
                    int ixA = (int)xA; if(ixA<0) ixA=0; if(ixA>=arr.largeur) ixA=arr.largeur-1;
                    int iyA = (int)yA; if(iyA<0) iyA=0; if(iyA>=arr.hauteur) iyA=arr.hauteur-1;

                    inter->pixels[y][x] = interpolation_pixel(dep.pixels[iyD][ixD], arr.pixels[iyA][ixA], alpha);
                    trouve = 1;
                    break;
                }
            }
            // fond noir si pas trouvé
            if (!trouve) {
                inter->pixels[y][x].r = 0; inter->pixels[y][x].g = 0; inter->pixels[y][x].b = 0;
            }
        }
    }
}