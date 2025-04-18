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
    FILE* fichier = fopen("kk.txt", "r");
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

int verifier_equipe_ko(Equipe e) {
    return (e.fighter_1.pv <= 0 && e.fighter_2.pv <= 0 && e.fighter_3.pv <= 0);
}

void jouer_tour(Equipe* equipe_actuelle, Equipe* equipe_adverse) {
    // Afficher les combattants disponibles
    printf("Combattants disponibles:\n");
    if (equipe_actuelle->fighter_1.pv > 0) printf("1. %s\n", equipe_actuelle->fighter_1.nom);
    if (equipe_actuelle->fighter_2.pv > 0) printf("2. %s\n", equipe_actuelle->fighter_2.nom);
    if (equipe_actuelle->fighter_3.pv > 0) printf("3. %s\n", equipe_actuelle->fighter_3.nom);
    
    // Choix du combattant
    int choix_combattant;
    do {
        printf("Choisissez un combattant (1-3): ");
        scanf("%d", &choix_combattant);
    } while (choix_combattant < 1 || choix_combattant > 3 || 
             (choix_combattant == 1 && equipe_actuelle->fighter_1.pv <= 0) ||
             (choix_combattant == 2 && equipe_actuelle->fighter_2.pv <= 0) ||
             (choix_combattant == 3 && equipe_actuelle->fighter_3.pv <= 0));
    
    Combattant* combattant_actuel;
    switch (choix_combattant) {
        case 1: combattant_actuel = &equipe_actuelle->fighter_1; break;
        case 2: combattant_actuel = &equipe_actuelle->fighter_2; break;
        case 3: combattant_actuel = &equipe_actuelle->fighter_3; break;
    }
    
    // Menu d'action
    printf("\nActions possibles pour %s:\n", combattant_actuel->nom);
    printf("1. Attaque de base\n");
    printf("2. Technique speciale - Attaque\n");
    printf("3. Technique speciale - Defense\n");
    printf("4. Technique speciale - Vitesse\n");
    
    int choix_action;
    do {
        printf("Choisissez une action (1-4): ");
        scanf("%d", &choix_action);
    } while (choix_action < 1 || choix_action > 4);
    
    // Exécuter l'action
    switch (choix_action) {
        case 1: attaque_de_base(combattant_actuel, equipe_adverse); break;
        case 2: utiliser_technique_speciale(combattant_actuel, &combattant_actuel->spe_attaque, equipe_adverse); break;
        case 3: utiliser_technique_speciale(combattant_actuel, &combattant_actuel->spe_defense, equipe_adverse); break;
        case 4: utiliser_technique_speciale(combattant_actuel, &combattant_actuel->spe_vitesse, equipe_adverse); break;
    }
}

void attaque_de_base(Combattant* attaquant, Equipe* equipe_adverse) {
    // Afficher les cibles possibles
    printf("\nCibles disponibles:\n");
    if (equipe_adverse->fighter_1.pv > 0) printf("1. %s (PV: %d)\n", equipe_adverse->fighter_1.nom, equipe_adverse->fighter_1.pv);
    if (equipe_adverse->fighter_2.pv > 0) printf("2. %s (PV: %d)\n", equipe_adverse->fighter_2.nom, equipe_adverse->fighter_2.pv);
    if (equipe_adverse->fighter_3.pv > 0) printf("3. %s (PV: %d)\n", equipe_adverse->fighter_3.nom, equipe_adverse->fighter_3.pv);
    
    // Choix de la cible
    int choix_cible;
    do {
        printf("Choisissez une cible (1-3): ");
        scanf("%d", &choix_cible);
    } while (choix_cible < 1 || choix_cible > 3 || 
             (choix_cible == 1 && equipe_adverse->fighter_1.pv <= 0) ||
             (choix_cible == 2 && equipe_adverse->fighter_2.pv <= 0) ||
             (choix_cible == 3 && equipe_adverse->fighter_3.pv <= 0));
    
    Combattant* cible;
    switch (choix_cible) {
        case 1: cible = &equipe_adverse->fighter_1; break;
        case 2: cible = &equipe_adverse->fighter_2; break;
        case 3: cible = &equipe_adverse->fighter_3; break;
    }
    
    // Calcul des dégâts
    int degats = attaquant->attaque - (cible->defense / 2);
    if (degats < 1) degats = 1; // Dégâts minimum
    
    cible->pv -= degats;
    printf("%s attaque %s et inflige %d degats!\n", attaquant->nom, cible->nom, degats);
    
    if (cible->pv <= 0) {
        printf("%s est K.O.!\n", cible->nom);
    }
}

void utiliser_technique_speciale(Combattant* utilisateur, TechniqueSpeciale* tech, Equipe* equipe_adverse) {
    // Vérifier si la technique est disponible
    if (tech->rechargement > 0) {
        printf("Technique %s n'est pas encore rechargée (%d tours restants)\n", tech->nom, tech->rechargement);
        return;
    }
    
    // Appliquer l'effet selon le type de cible
    if (strcmp(tech->type_cible, "ennemi") == 0) {
        // Cibler un ennemi
        printf("\nCibles ennemies disponibles:\n");
        if (equipe_adverse->fighter_1.pv > 0) printf("1. %s (PV: %d)\n", equipe_adverse->fighter_1.nom, equipe_adverse->fighter_1.pv);
        if (equipe_adverse->fighter_2.pv > 0) printf("2. %s (PV: %d)\n", equipe_adverse->fighter_2.nom, equipe_adverse->fighter_2.pv);
        if (equipe_adverse->fighter_3.pv > 0) printf("3. %s (PV: %d)\n", equipe_adverse->fighter_3.nom, equipe_adverse->fighter_3.pv);
        
        int choix_cible;
        do {
            printf("Choisissez une cible (1-3): ");
            scanf("%d", &choix_cible);
        } while (choix_cible < 1 || choix_cible > 3 || 
                 (choix_cible == 1 && equipe_adverse->fighter_1.pv <= 0) ||
                 (choix_cible == 2 && equipe_adverse->fighter_2.pv <= 0) ||
                 (choix_cible == 3 && equipe_adverse->fighter_3.pv <= 0));
        
        Combattant* cible;
        switch (choix_cible) {
            case 1: cible = &equipe_adverse->fighter_1; break;
            case 2: cible = &equipe_adverse->fighter_2; break;
            case 3: cible = &equipe_adverse->fighter_3; break;
        }
        
        // Appliquer l'effet
        cible->pv -= tech->valeur;
        printf("%s utilise %s sur %s: %s!\n", utilisateur->nom, tech->nom, cible->nom, tech->description);
        printf("%s perd %d PV!\n", cible->nom, tech->valeur);
        
        if (cible->pv <= 0) {
            printf("%s est K.O.!\n", cible->nom);
        }
    } else {
        // Cibler un allié ou soi-même (implémentation similaire)
        // ...
    }
    
    // Mettre en rechargement la technique
    tech->rechargement = tech->rechargement;
}

void afficher_combat(Equipe e1, Equipe e2) {
    printf("\n========================== COMBAT ==========================\n");
    printf("| %-25s | %-25s |\n", e1.Nom_equipe, e2.Nom_equipe);
    printf("|---------------------------|---------------------------|\n");
    
    // Combattant 1
    printf("| 1. %-22s | 1. %-22s |\n", 
           (e1.fighter_1.pv > 0) ? e1.fighter_1.nom : "[K.O.]", 
           (e2.fighter_1.pv > 0) ? e2.fighter_1.nom : "[K.O.]");
    printf("|   PV:%-4d Att:%-3d Def:%-3d |   PV:%-4d Att:%-3d Def:%-3d |\n",
           (e1.fighter_1.pv > 0) ? e1.fighter_1.pv : 0,
           e1.fighter_1.attaque, e1.fighter_1.defense,
           (e2.fighter_1.pv > 0) ? e2.fighter_1.pv : 0,
           e2.fighter_1.attaque, e2.fighter_1.defense);
    
    // Combattant 2
    printf("| 2. %-22s | 2. %-22s |\n", 
           (e1.fighter_2.pv > 0) ? e1.fighter_2.nom : "[K.O.]", 
           (e2.fighter_2.pv > 0) ? e2.fighter_2.nom : "[K.O.]");
    printf("|   PV:%-4d Att:%-3d Def:%-3d |   PV:%-4d Att:%-3d Def:%-3d |\n",
           (e1.fighter_2.pv > 0) ? e1.fighter_2.pv : 0,
           e1.fighter_2.attaque, e1.fighter_2.defense,
           (e2.fighter_2.pv > 0) ? e2.fighter_2.pv : 0,
           e2.fighter_2.attaque, e2.fighter_2.defense);
    
    // Combattant 3
    printf("| 3. %-22s | 3. %-22s |\n", 
           (e1.fighter_3.pv > 0) ? e1.fighter_3.nom : "[K.O.]", 
           (e2.fighter_3.pv > 0) ? e2.fighter_3.nom : "[K.O.]");
    printf("|   PV:%-4d Att:%-3d Def:%-3d |   PV:%-4d Att:%-3d Def:%-3d |\n",
           (e1.fighter_3.pv > 0) ? e1.fighter_3.pv : 0,
           e1.fighter_3.attaque, e1.fighter_3.defense,
           (e2.fighter_3.pv > 0) ? e2.fighter_3.pv : 0,
           e2.fighter_3.attaque, e2.fighter_3.defense);
    
    printf("===========================================================\n");
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

    // Début de l'implémentation du combat
    printf("\nLe combat commence!\n");
    
    int tour = 1;
    int equipe1_ko = 0, equipe2_ko = 0;
    
    while (!equipe1_ko && !equipe2_ko) {
        printf("\n=== TOUR %d ===\n", tour);
        afficher_combat(equipe1, equipe2);
        
        // Vérifier l'état des équipes
        equipe1_ko = verifier_equipe_ko(equipe1);
        equipe2_ko = verifier_equipe_ko(equipe2);
        
        if (equipe1_ko || equipe2_ko) break;
        
        // Tour de l'équipe 1
        printf("\n--- Tour de l'equipe %s ---\n", equipe1.Nom_equipe);
        jouer_tour(&equipe1, &equipe2);
        
        // Vérifier à nouveau après le tour
        equipe2_ko = verifier_equipe_ko(equipe2);
        if (equipe2_ko) break;
        
        // Tour de l'équipe 2
        printf("\n--- Tour de l'equipe %s ---\n", equipe2.Nom_equipe);
        jouer_tour(&equipe2, &equipe1);
        
        tour++;
    }
    
    // Annoncer le vainqueur
    if (equipe1_ko) {
        printf("\nL'equipe %s a gagne!\n", equipe2.Nom_equipe);
    } else {
        printf("\nL'equipe %s a gagne!\n", equipe1.Nom_equipe);
    }
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
