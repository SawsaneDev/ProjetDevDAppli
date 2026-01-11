#include "uvsqgraphics_2.h"
#include "morphing.h"
#include <stdio.h>
#include <stdlib.h>

// Fonction pour vérifier si un point P est à l'intérieur d'un triangle ABC
// Utilise les produits vectoriels pour tester le côté de chaque segment
 int point_dans_triangle(Point p, Point a, Point b, Point c){
    float d1= (p.x -b.x)*(a.y -b.y) -(a.x - b.x)*(p.y-b.y);
    float d2= (p.x -c.x)*(b.y -c.y) -(b.x - c.x)*(p.y-c.y);
    float d3= (p.x -a.x)*(c.y -a.y) -(c.x - a.x)*(p.y-a.y);
  // Le point est à l'intérieur si tous les signes des produits vectoriels sont les mêmes
    return ((d1>=0 && d2>=0 && d3>=0) || (d1<=0 && d2<=0 && d3<=0));
}

// Calcul des positions intermédiaires des points de base entre deux images
// dep : points de l'image de départ, arr : points de l'image d'arrivée
// inter : points intermédiaires calculés pour l'image k/N
void calcul_points_intermediaires(Point* dep, Point* arr, Point* inter, int n, float alpha){
    for (int i=0; i<n; i++){
        inter[i].x = (1-alpha)*dep[i].x + alpha*arr[i].x; // formule x intermédiaire
        inter[i].y = (1-alpha)*dep[i].y + alpha*arr[i].y; // formule y intermédiaire
    }
}

// Triangulation de l'image intermédiaire
// pts : points intermédiaires, tris : tableau des triangles, nb_tris : nombre de triangles calculés
void triangulation(Point* pts , int n, Triangle* tris, int* nb_tris){
  if(n<5){
   *nb_tris=0; // pas assez de points pour trianguler
   return;
  }
  // Construction des 4 premiers triangles à partir des 4 coins
    tris[0]=(Triangle){0,1,4};
    tris[1]=(Triangle){1,2,4};
    tris[2]=(Triangle){2,3,4};
    tris[3]=(Triangle){3,0,4};
    *nb_tris=4;
 
  // Pour chaque point restant, trouver le triangle contenant ce point et subdiviser
    for (int p=5; p<n; p++){
        for(int t=0; t< *nb_tris; t++){
            Triangle tr= tris[t];
         
            // Vérifie si le point est dans ce triangle
            if (point_dans_triangle(pts[p], pts[tr.p1], pts[tr.p2], pts[tr.p3])){
              // Supprimer le triangle original en le remplaçant par le dernier
                tris[t]= tris[--(*nb_tris)];
             
               // Ajouter les trois nouveaux triangles formés avec le nouveau point
                tris [(*nb_tris)++]= (Triangle){tr.p1, tr.p2, p};
                tris [(*nb_tris)++]= (Triangle){tr.p2, tr.p3, p};
                tris [(*nb_tris)++]= (Triangle){tr.p3, tr.p1, p};
                break;// on a trouvé le triangle, on passe au point suivant
            }       
        }

    }
}



// Interpolation linéaire entre deux pixels selon alpha
Pixel interpolation_pixel(Pixel d, Pixel a , float alpha){
    Pixel p;
    p.r = (1- alpha)* d.r + alpha* a.r;
    p.g = (1- alpha)* d.g + alpha* a.g;
    p.b = (1- alpha)* d.b + alpha* a.b;
    return p;
}

// Calcul des coordonnées barycentriques d'un point P par rapport au triangle ABC
// l1, l2, l3 sont les coefficients barycentriques : l1+l2+l3=1
void calcul_barycentrique(Point P,Point A, Point B, Point C, float* l1, float* l2, float* l3){
    float detT = (B.y -C.y)*(A.x-C.x) +(C.x-B.x)*(A.y-C.y);// déterminant du triangle
    *l1=((B.y -C.y)*(P.x-C.x)+(C.x-B.x)*(P.y-C.y))/ detT;
    *l2=((C.y-A.y)*(P.x-C.x)+(A.x-C.x)*(P.y-C.y))/detT;
    *l3= 1.0f-*l1-*l2; // somme des barycentriques = 1
}


// Calcul de l'image intermédiaire entre dep et arr
// k : étape actuelle, N : nombre total d'images
void calcul_image_intermediaire(Image dep, Image arr, Image* inter, int k, int N){
    float alpha= (float)k/ N; // coefficient d'interpolation

  // Copier les dimensions et nombre de points
    inter->largeur= dep.largeur;
    inter->hauteur= dep.hauteur;
    inter->nb_points=dep.nb_points;

  // Calcul des points intermédiaires
    calcul_points_intermediaires(dep.points, arr.points, inter->points, dep.nb_points, alpha);
 // Parcours de chaque pixel de l'image
    triangulation(inter->points, inter->nb_points, inter->triangles, &inter->nb_triangles);
 
// Parcours de chaque pixel de l'image
    for(int y=0; y<dep.hauteur; y++){
        for(int x=0; x< dep.largeur; x++){

      

            Point P={ (float)x, (float)y};
            int trouve =0; // flag pour savoir si le pixel appartient à un triangle
            for (int t=0; t< inter->nb_triangles && !trouve; t++){
                Triangle tr= inter->triangles[t];
             
                Point A= inter->points[tr.p1];
                Point B= inter->points[tr.p2];
                Point C= inter->points[tr.p3];

                float l1,l2,l3;
                calcul_barycentrique(P,A,B,C,&l1,&l2,&l3);

                if (l1>=0 && l2>=0 && l3 >=0){// le pixel est dans le triangle
                 // Calcul de la position correspondante dans l'image de départ
                    int xD= (int)(l1*dep.points[tr.p1].x +l2*dep.points[tr.p2].x + l3*dep.points[tr.p3].x);
                    int yD= (int)(l1*dep.points[tr.p1].y +l2*dep.points[tr.p2].y + l3*dep.points[tr.p3].y);
                  // Calcul de la position correspondante dans l'image d'arrivée
                    int xA= (int)(l1*arr.points[tr.p1].x +l2*arr.points[tr.p2].x + l3*arr.points[tr.p3].x);
                    int yA= (int)(l1*arr.points[tr.p1].y +l2*arr.points[tr.p2].y + l3*arr.points[tr.p3].y);

                  // Clamping : éviter de sortir de l'image
                    if (xD <0) xD=0; 
                    if (yD<0) yD=0;
                    if (xD >= dep.largeur) xD= dep.largeur-1;
                    if (yD>=dep.hauteur) yD=dep.hauteur-1;

                    if (xA<0) xA =0;
                    if (yA<0) yA=0;
                    if (xA >= arr.largeur) xA= arr.largeur-1;
                    if (yA>=arr.hauteur) yA=arr.hauteur-1;

                  // Interpolation de la couleur
                    inter->pixels[y][x]= interpolation_pixel(dep.pixels[yD][xD], arr.pixels[yA][xA], alpha);
                    trouve = 1;// pixel traité
                }

            }

        }
    }
}


 

