#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

void afficher_separateur(int longueur) {
    printf("+");
    for (int i = 0; i < longueur-2; i++) printf("-");
    printf("+\n");
}

void afficher_ligne_vide(int longueur) {
    printf("|");
    for (int i = 0; i < longueur-2; i++) printf(" ");
    printf("|\n");
}

void afficher_ligne_texte(int longueur, const char* texte) {
    printf("| %-*s |\n", longueur-4, texte);
}

void afficher_combat(Equipe e1, Equipe e2) {
    const int largeur = 80;
    
    printf("\n");
    afficher_separateur(largeur);
    printf("| %-76s |\n", "ONE PIECE FIGHT");
    afficher_separateur(largeur);
    
    // En-tête équipes
    printf("| %-36s | %-36s |\n", e1.Nom_equipe, e2.Nom_equipe);
    afficher_separateur(largeur);
    
    // Combattant 1
    printf("| 1. %-22s (PV: %-4d) | 1. %-22s (PV: %-4d) |\n",
           e1.fighter_1->pv > 0 ? e1.fighter_1->nom : "[K.O.]", e1.fighter_1->pv,
           e2.fighter_1->pv > 0 ? e2.fighter_1->nom : "[K.O.]", e2.fighter_1->pv);
    
    // Combattant 2
    printf("| 2. %-22s (PV: %-4d) | 2. %-22s (PV: %-4d) |\n",
           e1.fighter_2->pv > 0 ? e1.fighter_2->nom : "[K.O.]", e1.fighter_2->pv,
           e2.fighter_2->pv > 0 ? e2.fighter_2->nom : "[K.O.]", e2.fighter_2->pv);
    
    // Combattant 3
    printf("| 3. %-22s (PV: %-4d) | 3. %-22s (PV: %-4d) |\n",
           e1.fighter_3->pv > 0 ? e1.fighter_3->nom : "[K.O.]", e1.fighter_3->pv,
           e2.fighter_3->pv > 0 ? e2.fighter_3->nom : "[K.O.]", e2.fighter_3->pv);
    
    afficher_separateur(largeur);
}