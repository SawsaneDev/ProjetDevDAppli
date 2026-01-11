#include "uvsqgraphics_2.h"
#include "morphing.h"
#include <stdio.h>
#include <stdlib.h>




void calcul_points_intermediaire(Point* dep, Point* arr, Point*inter, int n, float alpha){
    for (int i=0;i<n; i++){
        inter[i].x= (1-alpha)*dep[i].x + alpha*arr[i].x;
        inter[i].y= (1-alpha)*dep[i].y + alpha*arr[i].y;
    }
}


 int  point_dans_triangle(Point p, Point a, Point b, Point c){
    float d1= (p.x -b.x)*(a.y -b.y) -(a.x - b.x)*(p.y-b.y);
    float d2= (p.x -c.x)*(b.y -c.y) -(b.x - c.x)*(p.y-c.y);
    float d3= (p.x -a.x)*(c.y -a.y) -(c.x - a.x)*(p.y-a.y);
    return ((d1>=0 && d2>=0 && d3>=0) || (d1<=0 && d2<=0 && d3<=0));
}

void triangulation(Point* pts , int n, Triangle* tris, int* nb_tris){
    tris[0]=(Triangle){0,1,4};
    tris[1]=(Triangle){1,2,4};
    tris[2]=(Triangle){2,3,4};
    tris[3]=(Triangle){3,0,4};
    *nb_tris=4;

    for (int p=5; p<n; p++){
        for(int t=0; t< *nb_tris; t++){
            Triangle tr= tris[t];
            if (point_dans_triangle(pts[p], pts[tr.a], pts[tr.b], pts[tr.c])){
                tris[t]= tris[--(*nb_tris)];
                tris [(*nb_tris)++]= (Triangle){tr.a, tr.b, p};
                tris [(*nb_tris)++]= (Triangle){tr.b, tr.c, p};
                tris [(*nb_tris)++]= (Triangle){tr.c, tr.a, p};
                break;
            }       
        }

    }
}



Pixel interpolation_pixel(Pixel d, Pixel a , float alpha){
    Pixel p;
    p.r = (1- alpha)* d.r + alpha* a.r;
    p.g = (1- alpha)* d.g + alpha* a.g;
    p.b = (1- alpha)* d.b + alpha* a.b;
    return p;
}


void calcul_barycentrique(Point P,Point A, Point B, Point C, float* l1, float* l2, float* l3){
    float detT = (B.y -C.y)*(A.x-C.x) +(C.x-B.x)*(A.y-C.y);
    *l1=((B.y -C.y)*(P.x-C.x)+(C.x-B.x)*(P.y-C.y))/ detT;
    *l2=((C.y-A.y)*(P.x-C.x)+(A.x-C.x)*(P.y-C.y))/detT;
    *l3= 1.0f-*l1-*l2;
}



void calcul_image_intermediaire(Image dep, Image arr, Image* inter, int k, int N){
    float alpha= (float)k/ N;
    calcul_points_intermediaire(dep.points, arr.points, inter->points, dep.nb_points, alpha);
    triangulation(inter->points, inter->nb_points, inter->triangles, &inter->nb_triangles);
    for(int y=0; y<dep.hauteur; y++){
        for(int x=0; x< dep.largeur; x++){
            Point P={ (float)x, (float)y};
            int trouve =0;
            for (int t=0; t< inter->nb_triangles && !trouve; t++){
                Triangle tr= inter->triangles[t];
                Point A= inter->points[tr.a];
                Point B= inter->points[tr.b];
                Point C= inter->points[tr.c];

                float l1,l2,l3;
                calcul_barycentrique(P,A,B,C,&l1,&l2,&l3);

                if (l1>=0 && l2>=0 && l3 >=0){
                    int xD= (int)(l1*dep.points[tr.a].x +l2*dep.points[tr.b].x + l3*dep.points[tr.c].x);
                    int yD= (int)(l1*dep.points[tr.a].y +l2*dep.points[tr.b].y + l3*dep.points[tr.c].y);
                    int xA= (int)(l1*arr.points[tr.a].x +l2*arr.points[tr.b].x + l3*arr.points[tr.c].x);
                    int yA= (int)(l1*arr.points[tr.a].y +l2*arr.points[tr.b].y + l3*arr.points[tr.c].y);

                    if (xD <0) xD=0; if (yD<0) yD=0;
                    if (xA<0) xA =0; if (yA<0) yA=0;
                    if (xD >= dep.largeur xD= dep.largeur-1; if (yD>=dep.hauteur) yD=dep.hauteur-1;
                    if (xA >= arr.largeur xA= arr.largeur-1; if (yA>=arr.hauteur) yA=arr.hauteur-1;

                    inter->pixels[y][x]= interpolation_pixel(dep.pixels[yD][xD], arr.pixels[yA][xA], alpha);
                    trouve = 1;
                }

            }
        }
    }
}

void generer_images_intermediaires(Image dep, Image arr, int N){
    Image inter;
    for (int k=0;k<=N;k++){
        calcul_image_intermediaire(dep,arr,&inter,k,N);
        char filename[64];
        sprintf (filename, "output/image_%03d.ppm", k);
        ecrire_ppm(inter, filename);
        printf("Image %d/%d generee.\n",k,N;
    }
}
    
