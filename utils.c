#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

// Affiche une ligne de séparation avec des '-'
void afficher_separateur(int longueur) {
    printf("+");
    for (int i = 0; i < longueur-2; i++) { 
        printf("-");
    }
    printf("+\n");
}

// Affiche une ligne vide entre des bordures '|'
void afficher_ligne_vide(int longueur) {
    printf("|");
    for (int i = 0; i < longueur-2; i++) {
        printf(" ");
    }
    printf("|\n");
}

// Affiche un texte centré dans un cadre
void afficher_ligne_texte(int longueur, const char* texte) {
    printf("| %-*s |\n", longueur-4, texte);
}

void afficher_combat(Equipe e1, Equipe e2) {
    const int largeur = 80;
    
    printf("\n");
    afficher_separateur(largeur);
    printf("| %-76s |\n", "ONE PIECE FIGHT");
    afficher_separateur(largeur);
    
    // Noms des équipes
    printf("| %-36s | %-36s |\n", e1.Nom_equipe, e2.Nom_equipe);
    afficher_separateur(largeur);
    
    // En-têtes des stats
    printf("|%-18s %3s %3s %3s %3s |%-18s %3s %3s %3s %3s |\n",
           "Combattant", "PV", "Att", "Def", "Agi",
           "Combattant", "PV", "Att", "Def", "Agi");
    
   
    
    // Format compact pour les combattants
    printf("|1.%-16s %3d %3d %3d %3d |1.%-16s %3d %3d %3d %3d |\n",
           e1.fighter_1->pv > 0 ? e1.fighter_1->nom : "[K.O.]", 
           e1.fighter_1->pv, e1.fighter_1->attaque,
           e1.fighter_1->defense, e1.fighter_1->agilite,
           e2.fighter_1->pv > 0 ? e2.fighter_1->nom : "[K.O.]", 
           e2.fighter_1->pv, e2.fighter_1->attaque,
           e2.fighter_1->defense, e2.fighter_1->agilite);
    
    printf("|2.%-16s %3d %3d %3d %3d |2.%-16s %3d %3d %3d %3d |\n",
           e1.fighter_2->pv > 0 ? e1.fighter_2->nom : "[K.O.]", 
           e1.fighter_2->pv, e1.fighter_2->attaque,
           e1.fighter_2->defense, e1.fighter_2->agilite,
           e2.fighter_2->pv > 0 ? e2.fighter_2->nom : "[K.O.]", 
           e2.fighter_2->pv, e2.fighter_2->attaque,
           e2.fighter_2->defense, e2.fighter_2->agilite);
    
    printf("|3.%-16s %3d %3d %3d %3d |3.%-16s %3d %3d %3d %3d |\n",
           e1.fighter_3->pv > 0 ? e1.fighter_3->nom : "[K.O.]", 
           e1.fighter_3->pv, e1.fighter_3->attaque,
           e1.fighter_3->defense, e1.fighter_3->agilite,
           e2.fighter_3->pv > 0 ? e2.fighter_3->nom : "[K.O.]", 
           e2.fighter_3->pv, e2.fighter_3->attaque,
           e2.fighter_3->defense, e2.fighter_3->agilite);
    
    afficher_separateur(largeur);
}
