#include "uvsqgraphics_2.h"


// Renvoie 1 si p est dans le triangle (A,B,C)
int dans_triangle2(POINT P, POINT A, POINT B, POINT C) {
	int vab, vbc, vca;
	vab = (A.x-P.x)*(B.y-P.y) - (A.y-P.y)*(B.x-P.x);
	vbc = (B.x-P.x)*(C.y-P.y) - (B.y-P.y)*(C.x-P.x);
	vca = (C.x-P.x)*(A.y-P.y) - (C.y-P.y)*(A.x-P.x);
	printf("%d %d %d\n",vab,vbc,vca);
	return 1;
}

int main(int argc, char *argv[]) {
	init_graphics(600,600);

	POINT B,R,V;
	
	B.x = 100; B.y = 100;
	R.x = 200; R.y = 300;
	V.x = 300; V.y = 100;
	draw_fill_triangle(B,R,V,blanc);
//	draw_triangle(P1,P2,P3,vert);
	draw_fill_circle(B,2,bleu);
	draw_fill_circle(R,2,rouge);
	draw_fill_circle(V,2,vert);
	int i;
	for (i=0 ; i<10 ; i++) {
		POINT P = wait_clic();
		dans_triangle2(P,B,V,R);
	}

	wait_escape();
	exit(0);
}
