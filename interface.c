#include "morphing.h"
#include "uvsqgraphics_2.h"
#include <stdio.h>
#include <stdlib.h>

// fonction pour tout redessiner a chaque frame
// c'est un peu lourd mais ca evite l'ecran noir
void redessiner_interface(Image *img1, Image *img2, int decale_x) {
    fill_screen(blanc);
    
    // image 1 a gauche
    for (int i = 0; i < img1->hauteur; i++) {
        for (int j = 0; j < img1->largeur; j++) {
            COULEUR c = couleur_RGB(img1->pixels[i][j].r, img1->pixels[i][j].g, img1->pixels[i][j].b);
            draw_pixel((POINT){j, i}, c);
        }
    }
    // image 2 a droite
    for (int i = 0; i < img2->hauteur; i++) {
        for (int j = 0; j < img2->largeur; j++) {
            COULEUR c = couleur_RGB(img2->pixels[i][j].r, img2->pixels[i][j].g, img2->pixels[i][j].b);
            draw_pixel((POINT){j + decale_x, i}, c);
        }
    }

    int y_boutons = (img1->hauteur > img2->hauteur ? img1->hauteur : img2->hauteur) + 20;

    // bouton vert (sauvegarder)
    POINT p1_vert = {20, y_boutons};
    POINT p2_vert = {140, y_boutons + 40};
    draw_fill_rectangle(p1_vert, p2_vert, vert);

    // bouton rouge (quitter)
    POINT p1_rouge = {160, y_boutons};
    POINT p2_rouge = {280, y_boutons + 40};
    draw_fill_rectangle(p1_rouge, p2_rouge, rouge);
}

// ouvre la fenetre graphique
void visualiser_image(Image img1, Image img2) {
    int w = img1.largeur + img2.largeur + 20;
    int h = (img1.hauteur > img2.hauteur) ? img1.hauteur : img2.hauteur;

    // +100 px pour avoir de la place pour les boutons en bas
    init_graphics(w, h + 100); 
    
    affiche_auto_off(); 
    redessiner_interface(&img1, &img2, img1.largeur + 20);
    affiche_all(); 
}

// ecrit les points dans le .txt
// ATTENTION: faut bien mettre les 4 coins au debut pour la triangulation
void sauver_points_dans_fichier(char *nom_fichier, Image img_dep, Image img_arr) {
    FILE *f = fopen(nom_fichier, "w");
    if (f == NULL) { printf("Erreur fichier.\n"); return; }
    
    // coin haut gauche, haut droit, bas droit, bas gauche
    fprintf(f, "0 0 0 0\n");
    fprintf(f, "%d 0 %d 0\n", img_dep.largeur-1, img_arr.largeur-1);
    fprintf(f, "%d %d %d %d\n", img_dep.largeur-1, img_dep.hauteur-1, img_arr.largeur-1, img_arr.hauteur-1);
    fprintf(f, "0 %d 0 %d\n", img_dep.hauteur-1, img_arr.hauteur-1);

    // boucle sur tous les points ajoutes
    for (int j = 0; j < img_dep.nb_points; j++) {
        fprintf(f, "%.0f %.0f %.0f %.0f\n",
            img_dep.points[j].x, img_dep.points[j].y,
            img_arr.points[j].x, img_arr.points[j].y);
    }
    fclose(f);
    printf("C'est bon, sauvegarde dans %s\n", nom_fichier);
}

// La grosse fonction de saisie
// gere les clics, l'affichage et les boutons
void saisir_les_points(Image *img_dep, Image *img_arr) {
    int n = 0;
    int decale_x = img_dep->largeur + 20;
    int h_max = (img_dep->hauteur > img_arr->hauteur) ? img_dep->hauteur : img_arr->hauteur;
    int y_boutons = h_max + 20; 

    printf("\n--- INTERFACE ---\n");
    printf("[VERT] = Sauver | [ROUGE] = Quitter\n");

    // boucle infinie tant qu'on clique pas sur quitter
    while(1) {
        
        affiche_auto_off();
        // on redessine le fond a chaque fois
        redessiner_interface(img_dep, img_arr, decale_x);
        
        // on reaffiche tous les points deja cliqués
        for(int k=0; k<n; k++) {
            POINT p1 = {(int)img_dep->points[k].x, (int)img_dep->points[k].y};
            POINT p2 = {(int)img_arr->points[k].x + decale_x, (int)img_arr->points[k].y};
            
            draw_fill_circle(p1, 3, rouge);
            // petit numero a cote du point
            aff_int(k, 15, (POINT){p1.x + 10, p1.y - 10}, rouge);

            draw_fill_circle(p2, 3, rouge);
            aff_int(k, 15, (POINT){p2.x + 10, p2.y - 10}, rouge);
            
            draw_line(p1, p2, bleu);
        }
        affiche_all(); 

        // on attend que l'utilisateur clique
        POINT p = wait_clic(); 

        // test si clic sur le bouton vert
        if (p.y >= y_boutons && p.y <= y_boutons+40 && p.x >= 20 && p.x <= 140) {
            sauver_points_dans_fichier("mes_points.txt", *img_dep, *img_arr);
            continue; // on reprend la boucle
        }
        // test si clic sur le bouton rouge
        if (p.y >= y_boutons && p.y <= y_boutons+40 && p.x >= 160 && p.x <= 280) {
            printf("Fin de la saisie, on lance le calcul.\n");
            break; // sortie de la boucle while
        }

        // sinon c'est peut etre un clic sur l'image de gauche
        if (p.x < img_dep->largeur && p.y < img_dep->hauteur) {
            
            // faut redessiner pour voir le point rouge temporaire
            affiche_auto_off(); 
            redessiner_interface(img_dep, img_arr, decale_x); 
            
            // repaint les anciens
            for(int k=0; k<n; k++) {
                POINT p1 = {(int)img_dep->points[k].x, (int)img_dep->points[k].y};
                POINT p2 = {(int)img_arr->points[k].x + decale_x, (int)img_arr->points[k].y};
                draw_fill_circle(p1, 3, rouge);
                aff_int(k, 15, (POINT){p1.x + 10, p1.y - 10}, rouge);
                draw_fill_circle(p2, 3, rouge);
                aff_int(k, 15, (POINT){p2.x + 10, p2.y - 10}, rouge);
                draw_line(p1, p2, bleu);
            }
            
            // dessine le nouveau point en cours
            draw_fill_circle(p, 3, rouge); 
            aff_int(n, 15, (POINT){p.x + 10, p.y - 10}, rouge);
            affiche_all(); 
            
            printf("Point %d a Gauche OK. Cliquez a DROITE mtn.\n", n);

            // boucle d'attente pour le deuxieme clic (a droite)
            while(1) {
                POINT p2 = wait_clic();
                
                // si on clique en bas ca compte pas
                if (p2.y >= y_boutons) continue; 
                
                // verif qu'on est bien a droite
                if (p2.x > decale_x) {
                    // on stocke tout dans les structures
                    img_dep->points[n].x = p.x;
                    img_dep->points[n].y = p.y;
                    img_arr->points[n].x = p2.x - decale_x;
                    img_arr->points[n].y = p2.y;
                    n++;
                    img_dep->nb_points = n;
                    img_arr->nb_points = n;
                    printf("Couple %d validé.\n", n - 1);
                    break; // on sort de la petite boucle
                }
            }
        }
    }
}