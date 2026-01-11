#include "uvsqgraphics_2.h"
#include "morphing.h"
#include <stdio.h>
#include <stdlib.h>

 int point_dans_triangle(Point p, Point a, Point b, Point c){
    float d1= (p.x -b.x)*(a.y -b.y) -(a.x - b.x)*(p.y-b.y);
    float d2= (p.x -c.x)*(b.y -c.y) -(b.x - c.x)*(p.y-c.y);
    float d3= (p.x -a.x)*(c.y -a.y) -(c.x - a.x)*(p.y-a.y);
    return ((d1>=0 && d2>=0 && d3>=0) || (d1<=0 && d2<=0 && d3<=0));
}

void calcul_points_intermediaires(Point* dep, Point* arr, Point* inter, int n, float alpha){
    for (int i=0; i<n; i++){
        inter[i].x = (1-alpha)*dep[i].x + alpha*arr[i].x;
        inter[i].y = (1-alpha)*dep[i].y + alpha*arr[i].y;
    }
}

void triangulation(Point* pts , int n, Triangle* tris, int* nb_tris){
    tris[0]=(Triangle){0,1,2};
    tris[1]=(Triangle){0,2,3};
    *nb_tris=2;

    for (int p=4; p<n; p++){
        for(int t=0; t< *nb_tris; t++){
            Triangle tr= tris[t];

            if (point_dans_triangle(pts[p], pts[tr.p1], pts[tr.p2], pts[tr.p3])){
                tris[t]= tris[--(*nb_tris)];

                tris [(*nb_tris)++]= (Triangle){tr.p1, tr.p2, p};
                tris [(*nb_tris)++]= (Triangle){tr.p2, tr.p3, p};
                tris [(*nb_tris)++]= (Triangle){tr.p3, tr.p1, p};
                break;
            }       
        }

    }
}

void calcul_image_intermediaire(Image dep, Image arr, Image* inter, int k, int N){
    float alpha= (float)k/ N;

    inter->largeur= dep.largeur;
    inter->hauteur= dep.hauteur;
    inter->nb_points=dep.nb_points;

    calcul_points_intermediaires(dep.points, arr.points, inter->points, dep.nb_points, alpha);
    triangulation(inter->points, inter->nb_points, inter->triangles, &inter->nb_triangles);

    for(int y=0; y<dep.hauteur; y++){
        for(int x=0; x< dep.largeur; x++){
            inter->pixels[y][x].r=(1-alpha)*dep.pixels[y][x].r + alpha*arr.pixels[y][x].r;
            inter->pixels[y][x].g=(1-alpha)*dep.pixels[y][x].g + alpha*arr.pixels[y][x].g;
            inter->pixels[y][x].b=(1-alpha)*dep.pixels[y][x].b + alpha*arr.pixels[y][x].b;
        }
    }
}



