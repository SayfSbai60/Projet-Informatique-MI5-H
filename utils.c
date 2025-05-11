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
    
    // En-têtes modifiés pour plus de clarté
    printf("|%-18s %3s %4s %7s %7s |%-18s %3s %4s %7s %7s |\n",
           "Combattant", "PV", "Att", "Def", "Agi",
           "Combattant", "PV", "Att", "Def", "Agi");
    
    // Affiche chaque combattant avec base + bonus
    for (int i = 0; i < 3; i++) {
        Combattant* f1 = (i == 0) ? e1.fighter_1 : (i == 1) ? e1.fighter_2 : e1.fighter_3;
        Combattant* f2 = (i == 0) ? e2.fighter_1 : (i == 1) ? e2.fighter_2 : e2.fighter_3;
        
        printf("|%d.%-16s %3d %4d %3d+%-3d %3d+%-3d |%d.%-16s %3d %4d %3d+%-3d %3d+%-3d |\n",
               i+1, 
               f1->pv > 0 ? f1->nom : "[K.O.]", 
               f1->pv > 0 ? f1->pv : 0,
               f1->attaque,
               f1->defense - f1->effets.defense_boost, f1->effets.defense_boost,
               f1->agilite - f1->effets.agilite_boost, f1->effets.agilite_boost,
               i+1,
               f2->pv > 0 ? f2->nom : "[K.O.]", 
               f2->pv > 0 ? f2->pv : 0,
               f2->attaque,
               f2->defense - f2->effets.defense_boost, f2->effets.defense_boost,
               f2->agilite - f2->effets.agilite_boost, f2->effets.agilite_boost);
    }
    afficher_separateur(largeur);
}
