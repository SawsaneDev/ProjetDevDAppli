#include "uvsqgraphics_2.h"


typedef struct{
    unsigned char r,g,b;
}Pixel;

typedef struct{
    float x,y;
}Points;

 typedef struct{
    int a, b, c;
}Triangle;

 typedef struct{
    int largeur, hauteur;
    Couleur** pixels;
    Point** points;
    int nb_points;
    Triangle* triangles;
    int nb_triangles;
}Images;

 int  point_dans_triangle(Point p, Point a, Point b, Point c){
    float d1= (p.x -b.x)*(a.y -b.y) -(a.x - b.x)*(p.y-b.y);
    float d2= (p.x -c.x)*(b.y -c.y) -(b.x - c.x)*(p.y-c.y);
    float d3= (p.x -a.x)*(c.y -a.y) -(c.x - a.x)*(p.y-a.y);
    return ((d1>=0 && d2>=0 && d3>=0) || (d1<=0 && d2<=0 && d3<=0));
}

void calcul_points_intermediaire(Point* dep, Point* arr, Point*inter, int n, float alpha){
    for (int i=0;i<n; i++){
        inter[i].x= (1-alpha)*dep[i].x + alpha*arr[i].x;
        inter[i].y= (1-alpha)*dep[i].y + alpha*arr[i].y;
    }
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

void afficher_image(Image img, int offsetX, int offsetY){
    for(int y=0; y< img.hauteur; y++){
        for(int x=0; x< img.largeur; x++){
            draw_pixel(offsetX+x, offsetY+y, img.pixels[y][x]);
        }
    }
}

void calcul_image_intermedaire(Image dep, Image arr, Image* inter, int k, int N){
    float alpha= (float)k/ N;
    calcul_points_intermediaire(dep.points, arr.points, inter->points, dep.nb_points, alpha);
    triangulation(inter->points, inter->nb_points, inter->triangles, &inter->nb_triangles);
    for(int y=0; y<dep.hauteur; y++){
        for(int x=0; x< dep.largeur; x++){
            inter->pixels[y][x].R=(1-alpha)*dep.pixels[y][x].R + alpha*arr.pixels[y][x].R;
            inter->pixels[y][x].V=(1-alpha)*dep.pixels[y][x].V + alpha*arr.pixels[y][x].V;
            inter->pixels[y][x].B=(1-alpha)*dep.pixels[y][x].B + alpha*arr.pixels[y][x].B;
        }
    }
}

void generer_images_intermediaires(Image dep, Image arr, int N){
    Image inter;
    inter.largeur= dep.largeur;
    inter.hauteur= dep.hauteur;
    inter.nb_points=dep.nb_points;
    inter.points= malloc(sizeof(Point) * dep.nb_points);
    inter.triangles= malloc (sizeof(Triangle) * (2*dep.nb_points -6));

    inter.pixels= malloc (sizeof(Couleur*) * dep.hauteur);
    for(int y =0; y<dep.hauteur; y++){
        inter.pixels[y]= malloc (sizeof(Couleur) * dep.largeur);
    }

    open_window(800,600);


    char filename[64];
    for (int k=0; k<=N; k++){
        calcul_image_intermedaire(dep, arr, &inter, k, N);

        clear_window(CouleurBlanche);
        afficher_image(dep,10,10);
        afficher_image(arr,220,10);
        afficher_image(inter,10,220);
        refresh_window();
        waitht_delay(200);

        sprintf (filename, "output/image_%03d.ppm", k);
        ecrire_ppm(inter, filename);
    }

    wait_escape();
    close_window();
    
    for (int y=0; y<dep.hauteur; y++) free(inter.pixels[y]);
    free(inter.pixels);
    free(inter.points);
    free(inter.triangles);
}
