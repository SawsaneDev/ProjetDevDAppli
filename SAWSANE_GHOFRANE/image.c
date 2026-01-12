#include "morphing.h"
#include "uvsqgraphics_2.h"
#include <stdio.h>
#include <stdlib.h>


/*Convertit une image (jpg,png...) en format PPM en utilisant la commande 
'convert' d'ImageMagick */
void conversion_ppm(char*input, char*output){
    char cmd[512];
    /* On prepare la chaine de caractere contenant la commande système */
    snprintf(cmd, sizeof(cmd),"convert %s %s", input, output);
    /* Appel au terminal pour éxécuter la conversion*/
    system(cmd);
}

/*Lit un fichier PPM de type P3 (format texte) et remplit la structure Image */
void lire_ppm(char* filename, Image* img) {

    FILE* f = fopen(filename,"r");
    if (!f) { printf("Erreur d'ouverture %s\n", filename); return; }
    
    char format[3];
    int max_val;
    int c;

    fscanf(f, "%2s", format);

    while (1) {
        c = fgetc(f);
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') continue;
    
    // Si on tombe sur un '#', on lit jusqu'à la fin de la ligne
        if (c == '#') {
            while (c != '\n' && c != EOF) c = fgetc(f);
            continue;
        }
        ungetc(c, f); // Ce n'était pas un commentaire, on remet le caractère
        break;
    }

    
    /* Lecture de l'en-tete du fichier PPM */
    if (fscanf(f, "%d %d %d", &img->largeur, &img->hauteur, &max_val) != 3) {
        printf("Erreur format\n");
        fclose(f);
        return;
    }

    // ImageMagick remet souvent un commentaire ici !
    while (1) {
        c = fgetc(f);
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') continue;
        if (c == '#') {
            while (c != '\n' && c != EOF) c = fgetc(f);
            continue;
        }
        ungetc(c, f); // On remet le premier chiffre du pixel
        break;
    }

    img->pixels = malloc(img->hauteur * sizeof(Pixel *));
    for (int i = 0; i < img->hauteur; i++) {
        img->pixels[i] = malloc(img->largeur * sizeof(Pixel));
    }

    /* On parcourt chaque ligne (i) puis chaque colonne (j)*/
    int pixel_count = 0;
    for(int i=0; i < img->hauteur; i++){
        for(int j=0; j < img->largeur;j++) {
            int r, g, b;

            /* Lecture des composantes Rouge, Vert, Bleu du pixel actuel*/
            if (fscanf(f,"%d %d %d",&r, &g, &b) == 3) {
                img->pixels[i][j].r= (unsigned char) r;
                img->pixels[i][j].g= (unsigned char) g;
                img->pixels[i][j].b=(unsigned char) b;
                pixel_count++;
            } else {
                img->pixels[i][j].r = 255;
                img->pixels[i][j].g = 0;
                img->pixels[i][j].b = 0;
            }
        }
    }

    /* Initialisation par défaut des compteurs de points et triangles */
    img->nb_points=0;
    img->nb_triangles=0;
    fclose(f);
}

void visualiser_image(Image img1, Image img2) {
    // 1. Init
    int w = img1.largeur + img2.largeur + 20;
    int h = img1.hauteur;
    if (img2.hauteur > h) h = img2.hauteur;

    init_graphics(w, h + 50);
    
    // --- DIAGNOSTIC DANS LE TERMINAL ---
    Pixel p = img2.pixels[img2.hauteur/2][img2.largeur/2];
    printf("--- TEST IMAGE 2 ---\n");
    printf("Couleur milieu Image 2 : R=%d V=%d B=%d\n", p.r, p.g, p.b);
    if (p.r == 0 && p.g == 0 && p.b == 0) {
        printf("ALERTE : L'image 2 est toute noire (problème de lecture fichier) !\n");
    }
    // -----------------------------------

    affiche_auto_off(); // On éteint l'affichage pour aller vite

    // 2. FOND ROUGE (Pour test)
    // Si la fenêtre reste noire, c'est que affiche_all() ne marche pas.
    // Si la fenêtre est rouge, l'affichage marche.
    fill_screen(blanc);

    // 3. Dessin Image 1 (Gauche)
    for (int i = 0; i < img1.hauteur; i++) {
        for (int j = 0; j < img1.largeur; j++) {
            COULEUR c = couleur_RGB(img1.pixels[i][j].r, img1.pixels[i][j].g, img1.pixels[i][j].b);
            draw_pixel((POINT){j, i}, c);
        }
    }

    // 4. Dessin Image 2 (Droite)
    for (int i = 0; i < img2.hauteur; i++) {
        for (int j = 0; j < img2.largeur; j++) {
            COULEUR c = couleur_RGB(img2.pixels[i][j].r, img2.pixels[i][j].g, img2.pixels[i][j].b);
            draw_pixel((POINT){j + img1.largeur + 20, i}, c);
        }
    }

    // 5. BOUTON D'AFFICHAGE
    affiche_all(); // <--- C'EST CA QUI AFFICHE TOUT D'UN COUP
    
    printf("Fenêtre ouverte. Cliquez pour continuer.\n");
    wait_clic();
}

/* Enregistre les données de l'image dans un nouveau fichier PPM (format P3) */
void sauver_img_ppm(char* filename, Image* img) {
    FILE* f = fopen(filename, "w");
    if (!f) return;
    /* Ecruture de l'en tete indispensable au format PPM*/
    fprintf(f, "P3\n%d %d\n255\n", img->largeur, img->hauteur);
    
    for(int i=0; i<img->hauteur; i++) {
        for(int j=0; j<img->largeur; j++){
            /*Ecriture des triplets R G B séparés par des espaces */
            fprintf(f, "%d %d %d", img->pixels[i][j].r, img->pixels[i][j].g, img->pixels[i][j].b);
        }
        /* Saut de ligne après chaque rangée de pixels pour que le fichier soit lisible */
        fprintf(f, "\n");
    }
    fclose(f);
}

void saisir_les_points(Image *img_dep, Image *img_arr) {
    int n = 0;
    int decale_x = img_dep->largeur + 20;
    int h_max = img_dep->hauteur;
    if (img_arr->hauteur > h_max) h_max = img_arr->hauteur;

    // --- PHASE 1 : DESSIN RAPIDE DU FOND ---
    affiche_auto_off(); // On éteint l'écran pour dessiner vite
    fill_screen(blanc);
    
    // Image Gauche
    for (int i = 0; i < img_dep->hauteur; i++) {
        for (int j = 0; j < img_dep->largeur; j++) {
            COULEUR c = couleur_RGB(img_dep->pixels[i][j].r, img_dep->pixels[i][j].g, img_dep->pixels[i][j].b);
            draw_pixel((POINT){j, i}, c);
        }
    }
    // Image Droite
    for (int i = 0; i < img_arr->hauteur; i++) {
        for (int j = 0; j < img_arr->largeur; j++) {
            COULEUR c = couleur_RGB(img_arr->pixels[i][j].r, img_arr->pixels[i][j].g, img_arr->pixels[i][j].b);
            draw_pixel((POINT){j + decale_x, i}, c);
        }
    }

    // Bouton STOP
    POINT p_bas_gauche = {0, h_max + 5};
    POINT p_bas_droite = {decale_x + img_arr->largeur, h_max + 50};
    draw_fill_rectangle(p_bas_gauche, p_bas_droite, gris);

    affiche_all(); // ON AFFICHE TOUT LE DÉCOR D'UN COUP
    
    // --- PHASE 2 : DESSIN DIRECT DES POINTS ---
    // C'EST LA LIGNE MAGIQUE QUI EMPÊCHE L'ÉCRAN NOIR :
    affiche_auto_on(); 
    // À partir d'ici, chaque draw_circle s'ajoute directement sur l'image existante
    // sans effacer le reste !

    printf("\n--- MODE SAISIE DES POINTS ---\n");
    printf("1. Cliquez à GAUCHE -> 2. Cliquez à DROITE.\n");
    printf("Pour FINIR : Cliquez dans la barre GRISE en bas.\n");

    while(1) {
        // --- CLIC GAUCHE ---
        POINT p1 = wait_clic();

        // Si clic en bas dans le gris -> STOP
        if (p1.y > h_max) {
            printf(">>> Fin de saisie. %d couples mémorisés.\n", n);
            break;
        }

        if (p1.x < img_dep->largeur) {
            // Dessin direct (plus besoin de affiche_all ici)
            draw_fill_circle(p1, 3, rouge);
            
            printf("Point %d (Gauche) OK.\n", n);

            // --- CLIC DROIT ---
            while(1) {
                POINT p2 = wait_clic();
                
                // Si annulation
                if (p2.y > h_max) { printf("Annulation et fin.\n"); return; }

                // Si erreur de côté
                if (p2.x < decale_x) {
                    printf("Erreur: Cliquez à DROITE !\n");
                    continue;
                }

                // C'est bon
                draw_fill_circle(p2, 3, rouge);
                draw_line(p1, p2, bleu); // On relie les points
                
                // Sauvegarde
                img_dep->points[n].x = p1.x;
                img_dep->points[n].y = p1.y;
                img_arr->points[n].x = p2.x - decale_x;
                img_arr->points[n].y = p2.y;

                n++;
                img_dep->nb_points = n;
                img_arr->nb_points = n;
                printf("Couple %d validé !\n", n);
                break; // On retourne au clic gauche
            }
        }
    }
}