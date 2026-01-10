#include "morphing.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    init_graphics(1200, 600);
    printf("Test du projet Morphing...\n")

    draw_fill_rect((POINT){0,0}, (PONIT){1200, 600}, blanc);
    draw_printf((POINT){450, 300}, noir, 20, "Projet Morphing Pret");

    draw_line((POINT){600,0}, (POINT){600, 600}, rouge);
    draw_prontf((POINT){200, 550}, bleu, 15, "Image de depart");
    draw_prontf((POINT){800, 550}, bleu, 15, "Image de depart");

    prontf("Cliquez dans la fenetre pour quittez.\n");
    wait_escape();

    return 0;
}











#include <stdio.h>
#include <stdlib.h>
#include "morphing.h"

int main(int argc, char *argv[]) {
    // Étape 1 : Vérifier si l'utilisateur a bien tapé les arguments
    // On attend : ./morphing image1.jpg image2.jpg 20
    if (argc < 4) {
        printf("Oups, il manque des arguments !\n");
        printf("Usage: %s Image_Depart Image_Arrivee N\n", argv[0]);
        return 1;
    }

    char *nom_dep = argv[1];
    char *nom_arr = argv[2];
    int N = atoi(argv[3]); // Nombre d'images intermédiaires [cite: 19]

    // Étape 2 : Préparer les images avec ImageMagick [cite: 37, 43]
    printf("Conversion des images en format PPM...\n");
    preparer_image(nom_dep, "depart.ppm");
    preparer_image(nom_arr, "arrivee.ppm");

    // Charger les images dans nos structures [cite: 45]
    Image img_depart, img_arrivee;
    lire_ppm("depart.ppm", &img_depart);
    lire_ppm("arrivee.ppm", &img_arrivee);

    // Étape 3 : Sélectionner les points (Partie interface) [cite: 23, 58]
    // Initialisation de la fenêtre graphique
    init_graphics(1200, 600); 
    afficher_deux_images(img_depart, img_arrivee); // [cite: 46]

    printf("Veuillez cliquer sur les points de correspondance...\n");
    // On appelle votre fonction de saisie ici
    saisir_les_points(&img_depart, &img_arrivee); 
    
    // Sauvegarder les points dans un fichier (n'oubliez pas les 4 coins !) [cite: 61, 62]
    sauver_points_dans_fichier("points_base.txt", img_depart, img_arrivee);

    // Étape 4 : Calculer les images intermédiaires [cite: 24, 66]
    printf("Generation des %d images intermédiaires...\n", N);
    for (int k = 0; k <= N; k++) {
        Image img_inter;
        
        // 4.1 Calculer la position des points [cite: 75]
        calculer_points_inter(&img_inter, img_depart, img_arrivee, k, N);
        
        // 4.2 Faire la triangulation [cite: 80]
        trianguler_image(&img_inter);
        
        // 4.3 Calculer la couleur des pixels [cite: 98]
        calculer_couleurs_pixels(&img_inter, img_depart, img_arrivee, k, N);
        
        // 4.4 Sauvegarder l'image k [cite: 108]
        char nom_img[50];
        sprintf(nom_img, "output/image_%03d.ppm", k);
        sauver_ppm(nom_img, &img_inter);
        
        printf("Image %d/%d terminee !\n", k, N);
    }

    // Étape 5 : Créer le film avec ffmpeg [cite: 110]
    printf("Creation du film final...\n");
    system("ffmpeg -i output/image_%03d.ppm -r 10 film_morphing.mp4");

    printf("Projet fini ! Appuyez sur Echap pour quitter.\n");
    wait_escape();
    return 0;
}