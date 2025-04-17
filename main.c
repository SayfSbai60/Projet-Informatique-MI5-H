#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LIGNE 512
#define MAX_COMBATTANTS 20

typedef struct {
    char nom[50];
    char description[100];
    int valeur;
    int duree;
    int rechargement;
    char type_cible[10];
} TechniqueSpeciale;

typedef struct {
    char nom[50];
    int pv;
    int attaque;
    int defense;
    int vitesse;
    TechniqueSpeciale spe_attaque;
    TechniqueSpeciale spe_defense;
    TechniqueSpeciale spe_vitesse;
} Combattant;

typedef struct {
    char Nom_equipe[100];
    Combattant fighter_1;
    Combattant fighter_2;
    Combattant fighter_3;
} Equipe;

Combattant* charger_combattants(unsigned int* nb_combattants) {
    FILE* fichier = fopen("combattants.txt", "r");
    if (!fichier) {
        printf("Erreur: Fichier 'combattants.txt' introuvable\n");
        exit(1);
    }

    Combattant* liste = malloc(MAX_COMBATTANTS * sizeof(Combattant));
    *nb_combattants = 0;
    char ligne[MAX_LIGNE];
    int numero;

    while (fgets(ligne, sizeof(ligne), fichier) && *nb_combattants < MAX_COMBATTANTS) {
        int result = sscanf(
            ligne, "%d;%49[^;];%d;%d;%d;%d;%49[^;];%99[^;];%d;%d;%d;%9[^;];%49[^;];%99[^;];%d;%d;%d;%9[^;];%49[^;];%99[^;];%d;%d;%d;%9[^\n]",
            &numero,
            liste[*nb_combattants].nom,
            &liste[*nb_combattants].pv,
            &liste[*nb_combattants].attaque,
            &liste[*nb_combattants].defense,
            &liste[*nb_combattants].vitesse,
            liste[*nb_combattants].spe_attaque.nom,
            liste[*nb_combattants].spe_attaque.description,
            &liste[*nb_combattants].spe_attaque.valeur,
            &liste[*nb_combattants].spe_attaque.duree,
            &liste[*nb_combattants].spe_attaque.rechargement,
            liste[*nb_combattants].spe_attaque.type_cible,
            liste[*nb_combattants].spe_defense.nom,
            liste[*nb_combattants].spe_defense.description,
            &liste[*nb_combattants].spe_defense.valeur,
            &liste[*nb_combattants].spe_defense.duree,
            &liste[*nb_combattants].spe_defense.rechargement,
            liste[*nb_combattants].spe_defense.type_cible,
            liste[*nb_combattants].spe_vitesse.nom,
            liste[*nb_combattants].spe_vitesse.description,
            &liste[*nb_combattants].spe_vitesse.valeur,
            &liste[*nb_combattants].spe_vitesse.duree,
            &liste[*nb_combattants].spe_vitesse.rechargement,
            liste[*nb_combattants].spe_vitesse.type_cible
        );

        if (result == 24) {
            (*nb_combattants)++;
        } else {
            printf("Erreur de format (%d champs lus): %s\n", result, ligne);
        }
    }

    fclose(fichier);
    return liste;
}

void afficher_details_combattant(Combattant c) {
    printf("\n=== %s ===\n", c.nom);
    printf("PV: %d | Att: %d | Def: %d | Vit: %d\n", c.pv, c.attaque, c.defense, c.vitesse);
    printf("Attaque speciale: %s - %s (%d pts, %d tours, rech: %d, cible: %s)\n",
           c.spe_attaque.nom, c.spe_attaque.description,
           c.spe_attaque.valeur, c.spe_attaque.duree,
           c.spe_attaque.rechargement, c.spe_attaque.type_cible);
    printf("Defense speciale: %s - %s (%d pts, %d tours, rech: %d, cible: %s)\n",
           c.spe_defense.nom, c.spe_defense.description,
           c.spe_defense.valeur, c.spe_defense.duree,
           c.spe_defense.rechargement, c.spe_defense.type_cible);
    printf("Vitesse speciale: %s - %s (%d pts, %d tours, rech: %d, cible: %s)\n",
           c.spe_vitesse.nom, c.spe_vitesse.description,
           c.spe_vitesse.valeur, c.spe_vitesse.duree,
           c.spe_vitesse.rechargement, c.spe_vitesse.type_cible);
}

void afficher_liste_combattants(Combattant* liste, int nb) {
    printf("\n=== LISTE DES COMBATTANTS ===\n");
    for (int i = 0; i < nb; i++) {
        printf("%2d. %-15s (PV: %3d, Att: %2d, Def: %2d, Vit: %3d)\n",
               i + 1,
               liste[i].nom,
               liste[i].pv,
               liste[i].attaque,
               liste[i].defense,
               liste[i].vitesse);
    }
}

Equipe creer_equipe(Combattant* liste, int nb_combattants, int num_equipe) {
    Equipe e;
    int choix;
    int combattants_choisis[3] = {0};

    printf("\n=== CREATION EQUIPE %d ===\n", num_equipe);
    printf("Nom de l'equipe: ");
    scanf("%99s", e.Nom_equipe);
    while(getchar() != '\n'); // Vide le buffer d'entrée

    afficher_liste_combattants(liste, nb_combattants);

    for (int i = 0; i < 3; i++) {
        do {
            printf("\nChoix du combattant %d (1-%d): ", i + 1, nb_combattants);
            if (scanf("%d", &choix) != 1) {
                printf("Entree invalide! Veuillez entrer un nombre.\n");
                while(getchar() != '\n'); // Vide le buffer d'entrée
                continue;
            }

            if (choix < 1 || choix > nb_combattants) {
                printf("Choix invalide! Veuillez choisir entre 1 et %d\n", nb_combattants);
                continue;
            }

            int deja_choisi = 0;
            for (int j = 0; j < i; j++) {
                if (combattants_choisis[j] == choix) {
                    deja_choisi = 1;
                    break;
                }
            }

            if (deja_choisi) {
                printf("Ce combattant est deja dans l'equipe!\n");
            } else {
                combattants_choisis[i] = choix;
                break;
            }
        } while (1);

        switch (i) {
            case 0: e.fighter_1 = liste[choix - 1]; break;
            case 1: e.fighter_2 = liste[choix - 1]; break;
            case 2: e.fighter_3 = liste[choix - 1]; break;
        }

        printf("%s ajoute a l'equipe!\n", liste[choix - 1].nom);
    }

    return e;
}

void afficher_equipe(Equipe e) {
    printf("\n=== EQUIPE: %s ===\n", e.Nom_equipe);
    printf("1. %s (PV: %d, Att: %d, Def: %d, Vit: %d)\n", 
           e.fighter_1.nom, e.fighter_1.pv, e.fighter_1.attaque, e.fighter_1.defense, e.fighter_1.vitesse);
    printf("2. %s (PV: %d, Att: %d, Def: %d, Vit: %d)\n", 
           e.fighter_2.nom, e.fighter_2.pv, e.fighter_2.attaque, e.fighter_2.defense, e.fighter_2.vitesse);
    printf("3. %s (PV: %d, Att: %d, Def: %d, Vit: %d)\n", 
           e.fighter_3.nom, e.fighter_3.pv, e.fighter_3.attaque, e.fighter_3.defense, e.fighter_3.vitesse);
}

void menu_pvp() {
    unsigned int nb_combattants;
    Combattant* liste = charger_combattants(&nb_combattants);

    printf("\n=== MODE PVP ===\n");
    Equipe equipe1 = creer_equipe(liste, nb_combattants, 1);
    Equipe equipe2 = creer_equipe(liste, nb_combattants, 2);

    printf("\n=== EQUIPES PRETES ===\n");
    afficher_equipe(equipe1);
    afficher_equipe(equipe2);

    free(liste);

    printf("\nLe combat va commencer...\n");
    // Logique du combat à ajouter ici
}

void nouvelle_partie() {
    int choix;
    do {
        printf("\n=== ONE PIECE FIGHT ===\n");
        printf("1. Mode PvP\n");
        printf("2. Mode PvE\n");
        printf("3. Quitter\n");
        printf("Choix: ");
        
        if (scanf("%d", &choix) != 1) {
            printf("Entree invalide!\n");
            while(getchar() != '\n');
            continue;
        }

        switch (choix) {
            case 1:
                menu_pvp();
                break;
            case 2:
                printf("Mode PvE non implémenté\n");
                break;
            case 3:
                printf("Au revoir!\n");
                exit(0);
            default:
                printf("Choix invalide!\n");
        }
    } while (1);
}

void menu_principal() {
    int choix;
    do {
        printf("\n=== MENU PRINCIPAL ===\n");
        printf("1. Nouvelle partie\n");
        printf("2. Quitter\n");
        printf("Choix: ");
        
        if (scanf("%d", &choix) != 1) {
            printf("Entree invalide!\n");
            while(getchar() != '\n');
            continue;
        }

        switch (choix) {
            case 1:
                nouvelle_partie();
                break;
            case 2:
                printf("Au revoir!\n");
                exit(0);
            default:
                printf("Choix invalide!\n");
        }
    } while (1);
}

int main() {
    menu_principal();
    return 0;
}
