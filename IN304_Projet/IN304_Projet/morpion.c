#include "uvsqgraphics_2.h"

// Contantes pour l'affichage
#define TAILLE_CASE 200
#define NBCASES 3
#define LARGEUR (NBCASES*TAILLE_CASE)
#define HAUTEUR (NBCASES*TAILLE_CASE)
#define COULEUR_LIGNE vert
#define COULEUR_J1 rouge
#define COULEUR_J2 bleu
#define COULEUR_VIDE jaune
#define RAYON (0.85*TAILLE_CASE/2)

//Constantes pour le jeu
#define JOUEUR1 1
#define JOUEUR2 2
#define VIDE 0
#define MATCH_NUL 0

// Variable globale pour stocker le jeu
int M[NBCASES][NBCASES];
int a_qui_de_jouer = JOUEUR1;

void initialiser_cases () {
	int i,j;
	for (i=0 ; i<NBCASES ; i++)
		for (j=0 ; j<NBCASES ; j++) 
			M[i][j] = VIDE;
//M[1][2] = JOUEUR1;
//M[2][0] = JOUEUR2;

}


int donner_etat(){
	return JOUEUR1;
}

POINT clic_case_vide(){
	POINT P;
	int encore=1;
	while(encore) {
printf("boucle encore\n");
		P = wait_clic();
		int i,j;
		i = P.x/TAILLE_CASE;
		j = P.y/TAILLE_CASE;
		if (M[i][j]==VIDE) encore=0;
		}
	return P;
}

void jouer(){
	POINT P = clic_case_vide();
	int i,j;
	i = P.x/TAILLE_CASE;
	j = P.y/TAILLE_CASE;
	if (a_qui_de_jouer==JOUEUR1)
		{
		M[i][j] = JOUEUR1;
		a_qui_de_jouer = JOUEUR2;
		}
		else  {
			M[i][j] = JOUEUR2;
			a_qui_de_jouer = JOUEUR1;
		}
}

void afficher_lignes_horizontales() {
	int i;
	POINT G,D;
	G.x = 0;
	D.x = LARGEUR;
	G.y = D.y = TAILLE_CASE;
	for (i=1 ; i<NBCASES ; i++) {
		draw_line(G,D,COULEUR_LIGNE);
		G.y += TAILLE_CASE;
		D.y = G.y;
	}
}

void afficher_lignes_verticaales() {
}

void afficher_lignes() {
	afficher_lignes_horizontales();
	afficher_lignes_verticaales();
}

void afficher_une_case(int i, int j) {
	POINT centre;
	centre.x = i*TAILLE_CASE + TAILLE_CASE/2;
	centre.y = j*TAILLE_CASE + TAILLE_CASE/2;
	COULEUR coul;
	if (M[i][j]==JOUEUR1) coul = COULEUR_J1;
	if (M[i][j]==JOUEUR2) coul = COULEUR_J2;
	if (M[i][j]==VIDE)    coul = COULEUR_VIDE;
	draw_fill_circle(centre,RAYON,coul);
}

void afficher_cases() {
	int i,j;
	for (i=0 ; i<NBCASES ; i++)
		for (j=0 ; j<NBCASES ; j++) 
			afficher_une_case(i,j);
}

void afficher_jeu() {
	afficher_lignes();
	afficher_cases();
	affiche_all();
}

void afficher_resultat() {
}

int main() {
	init_graphics(LARGEUR,HAUTEUR);
	affiche_auto_off();
	initialiser_cases ();
	afficher_jeu();
	while (donner_etat ()!=MATCH_NUL) {
		jouer();
		afficher_jeu();
	}
	afficher_resultat();
	attendre(5000);
	exit(0);
}
