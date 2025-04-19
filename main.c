#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

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
    int attaque_boost;
    int defense_boost;
    int vitesse_boost;
    int tours_restants;
} EffetsTemporaires;

typedef struct {
    char nom[50];
    int pv;
    int pv_max;
    int attaque;
    int defense;
    int agilite;
    int vitesse;
    TechniqueSpeciale spe_attaque;
    TechniqueSpeciale spe_defense;
    TechniqueSpeciale spe_vitesse;
    EffetsTemporaires effets;
    int cooldown_attaque;
    int cooldown_defense;
    int cooldown_vitesse;
} Combattant;

typedef struct {
    char Nom_equipe[100];
    Combattant* fighter_1;
    Combattant* fighter_2;
    Combattant* fighter_3;
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
        Combattant* c = &liste[*nb_combattants];
        
        int result = sscanf(
            ligne, "%d;%49[^;];%d;%d;%d;%d;%d;%49[^;];%99[^;];%d;%d;%d;%9[^;];%49[^;];%99[^;];%d;%d;%d;%9[^;];%49[^;];%99[^;];%d;%d;%d;%9[^\n]",
            &numero,
            c->nom,
            &c->pv,
            &c->pv_max,
            &c->attaque,
            &c->defense,
            &c->agilite,
            &c->vitesse,
            c->spe_attaque.nom,
            c->spe_attaque.description,
            &c->spe_attaque.valeur,
            &c->spe_attaque.duree,
            &c->spe_attaque.rechargement,
            c->spe_attaque.type_cible,
            c->spe_defense.nom,
            c->spe_defense.description,
            &c->spe_defense.valeur,
            &c->spe_defense.duree,
            &c->spe_defense.rechargement,
            c->spe_defense.type_cible,
            c->spe_vitesse.nom,
            c->spe_vitesse.description,
            &c->spe_vitesse.valeur,
            &c->spe_vitesse.duree,
            &c->spe_vitesse.rechargement,
            c->spe_vitesse.type_cible
        );

        if (result == 25) {
            // Initialiser les cooldowns et effets
            c->cooldown_attaque = 0;
            c->cooldown_defense = 0;
            c->cooldown_vitesse = 0;
            c->effets.attaque_boost = 0;
            c->effets.defense_boost = 0;
            c->effets.vitesse_boost = 0;
            c->effets.tours_restants = 0;
            (*nb_combattants)++;
        } else {
            printf("Erreur de format (%d champs lus): %s\n", result, ligne);
        }
    }

    fclose(fichier);
    return liste;
}

void appliquer_effets(Combattant* c) {
    if (c->effets.tours_restants > 0) {
        c->effets.tours_restants--;
        if (c->effets.tours_restants == 0) {
            c->attaque -= c->effets.attaque_boost;
            c->defense -= c->effets.defense_boost;
            c->vitesse -= c->effets.vitesse_boost;
            c->effets.attaque_boost = 0;
            c->effets.defense_boost = 0;
            c->effets.vitesse_boost = 0;
            printf("%s: Les effets temporaires ont expire.\n", c->nom);
        }
    }
}

void diminuer_cooldowns(Combattant* c) {
    if (c->cooldown_attaque > 0) c->cooldown_attaque--;
    if (c->cooldown_defense > 0) c->cooldown_defense--;
    if (c->cooldown_vitesse > 0) c->cooldown_vitesse--;
}

int verifier_equipe_ko(Equipe e) {
    return (e.fighter_1->pv <= 0 && e.fighter_2->pv <= 0 && e.fighter_3->pv <= 0);
}

Combattant* choisir_combattant_actif(Equipe* equipe) {
    printf("Combattants disponibles:\n");
    if (equipe->fighter_1->pv > 0) printf("1. %s\n", equipe->fighter_1->nom);
    if (equipe->fighter_2->pv > 0) printf("2. %s\n", equipe->fighter_2->nom);
    if (equipe->fighter_3->pv > 0) printf("3. %s\n", equipe->fighter_3->nom);
    
    int choix;
    do {
        printf("Choisissez un combattant (1-3): ");
        scanf("%d", &choix);
    } while (choix < 1 || choix > 3 || 
             (choix == 1 && equipe->fighter_1->pv <= 0) ||
             (choix == 2 && equipe->fighter_2->pv <= 0) ||
             (choix == 3 && equipe->fighter_3->pv <= 0));
    
    switch (choix) {
        case 1: return equipe->fighter_1;
        case 2: return equipe->fighter_2;
        case 3: return equipe->fighter_3;
        default: return NULL;
    }
}

Combattant* choisir_cible(Equipe* equipe, const char* type) {
    printf("\nCibles %s disponibles:\n", type);
    if (equipe->fighter_1->pv > 0) printf("1. %s (PV: %d)\n", equipe->fighter_1->nom, equipe->fighter_1->pv);
    if (equipe->fighter_2->pv > 0) printf("2. %s (PV: %d)\n", equipe->fighter_2->nom, equipe->fighter_2->pv);
    if (equipe->fighter_3->pv > 0) printf("3. %s (PV: %d)\n", equipe->fighter_3->nom, equipe->fighter_3->pv);
    
    int choix;
    do {
        printf("Choisissez une cible (1-3): ");
        scanf("%d", &choix);
    } while (choix < 1 || choix > 3 || 
             (choix == 1 && equipe->fighter_1->pv <= 0) ||
             (choix == 2 && equipe->fighter_2->pv <= 0) ||
             (choix == 3 && equipe->fighter_3->pv <= 0));
    
    switch (choix) {
        case 1: return equipe->fighter_1;
        case 2: return equipe->fighter_2;
        case 3: return equipe->fighter_3;
        default: return NULL;
    }
}

void attaque_de_base(Combattant* attaquant, Equipe* equipe_adverse) {
    Combattant* cible = choisir_cible(equipe_adverse, "ennemie");
    
    // Calcul de l'esquive basée sur l'agilité
    int chance_esquive = cible->agilite - attaquant->agilite;
    if (chance_esquive < 0) chance_esquive = 0;
    if (rand() % 100 < chance_esquive) {
        printf("%s esquive l'attaque grace a son agilite!\n", cible->nom);
        return;
    }
    
    // Calcul des dégâts avec pourcentage de réduction
    float reduction = (float)cible->defense / (cible->defense + 100);
    int degats = attaquant->attaque * (1 - reduction);
    if (degats < 1) degats = 1;
    
    cible->pv -= degats;
    printf("%s attaque %s et inflige %d degats! (Reduction: %.0f%%)\n", 
           attaquant->nom, cible->nom, degats, reduction*100);
    
    if (cible->pv <= 0) {
        cible->pv = 0;
        printf("%s est K.O.!\n", cible->nom);
    }
}

void appliquer_technique(Combattant* utilisateur, TechniqueSpeciale* tech, Combattant* cible) {
    if (strstr(tech->description, "Augmente") || strstr(tech->description, "Renforce")) {
        // Effet de boost positif
        if (strstr(tech->description, "attaque") || strstr(tech->description, "Attaque")) {
            cible->attaque += tech->valeur;
            cible->effets.attaque_boost = tech->valeur;
            printf("%s: Attaque augmentee de %d pour %d tours!\n", 
                   cible->nom, tech->valeur, tech->duree);
        }
        else if (strstr(tech->description, "defense") || strstr(tech->description, "Defense")) {
            cible->defense += tech->valeur;
            cible->effets.defense_boost = tech->valeur;
            printf("%s: Defense augmentee de %d pour %d tours!\n", 
                   cible->nom, tech->valeur, tech->duree);
        }
        else if (strstr(tech->description, "vitesse") || strstr(tech->description, "Vitesse")) {
            cible->vitesse += tech->valeur;
            cible->effets.vitesse_boost = tech->valeur;
            printf("%s: Vitesse augmentee de %d pour %d tours!\n", 
                   cible->nom, tech->valeur, tech->duree);
        }
        cible->effets.tours_restants = tech->duree;
    } 
    else if (strstr(tech->description, "Soin") || strstr(tech->description, "soin")) {
        // Effet de soin
        int nouveau_pv = cible->pv + tech->valeur;
        if (nouveau_pv > cible->pv_max) nouveau_pv = cible->pv_max;
        printf("%s soigne %s de %d PV! (%d -> %d)\n", 
               utilisateur->nom, cible->nom, tech->valeur, cible->pv, nouveau_pv);
        cible->pv = nouveau_pv;
    }
    else {
        // Dégâts normaux
        cible->pv -= tech->valeur;
        printf("%s utilise %s sur %s et inflige %d degats!\n", 
               utilisateur->nom, tech->nom, cible->nom, tech->valeur);
        if (cible->pv <= 0) {
            cible->pv = 0;
            printf("%s est K.O.!\n", cible->nom);
        }
    }
}

void utiliser_technique_speciale(Combattant* utilisateur, TechniqueSpeciale* tech, Equipe* equipe_adverse, Equipe* equipe_alliee) {
    // Techniques d'attaque ciblent toujours les ennemis
    if (tech == &utilisateur->spe_attaque) {
        Combattant* cible = choisir_cible(equipe_adverse, "ennemie");
        appliquer_technique(utilisateur, tech, cible);
    }
    // Techniques de défense/vitesse ciblent toujours les alliés
    else {
        Combattant* cible = choisir_cible(equipe_alliee, "alliee");
        appliquer_technique(utilisateur, tech, cible);
    }
    
    // Mettre en rechargement
    if (tech == &utilisateur->spe_attaque) {
        utilisateur->cooldown_attaque = tech->rechargement;
    } else if (tech == &utilisateur->spe_defense) {
        utilisateur->cooldown_defense = tech->rechargement;
    } else if (tech == &utilisateur->spe_vitesse) {
        utilisateur->cooldown_vitesse = tech->rechargement;
    }
}

void jouer_tour(Equipe* equipe_actuelle, Equipe* equipe_adverse) {
    Combattant* combattant_actuel = choisir_combattant_actif(equipe_actuelle);
    
    printf("\nActions possibles pour %s:\n", combattant_actuel->nom);
    printf("1. Attaque de base\n");
    printf("2. Technique speciale - Attaque (%s)", combattant_actuel->spe_attaque.nom);
    if (combattant_actuel->cooldown_attaque > 0) printf(" (Recharge: %d)", combattant_actuel->cooldown_attaque);
    printf("\n");
    printf("3. Technique speciale - Defense (%s)", combattant_actuel->spe_defense.nom);
    if (combattant_actuel->cooldown_defense > 0) printf(" (Recharge: %d)", combattant_actuel->cooldown_defense);
    printf("\n");
    printf("4. Technique speciale - Vitesse (%s)", combattant_actuel->spe_vitesse.nom);
    if (combattant_actuel->cooldown_vitesse > 0) printf(" (Recharge: %d)", combattant_actuel->cooldown_vitesse);
    printf("\n");
    
    int choix_action;
    do {
        printf("Choisissez une action (1-4): ");
        scanf("%d", &choix_action);
    } while (choix_action < 1 || choix_action > 4);
    
    switch (choix_action) {
        case 1: 
            attaque_de_base(combattant_actuel, equipe_adverse); 
            break;
        case 2: 
            if (combattant_actuel->cooldown_attaque > 0) {
                printf("Technique pas encore prete! (%d tours restants)\n", combattant_actuel->cooldown_attaque);
                jouer_tour(equipe_actuelle, equipe_adverse);
                return;
            }
            utiliser_technique_speciale(combattant_actuel, &combattant_actuel->spe_attaque, equipe_adverse, equipe_actuelle); 
            break;
        case 3: 
            if (combattant_actuel->cooldown_defense > 0) {
                printf("Technique pas encore prete! (%d tours restants)\n", combattant_actuel->cooldown_defense);
                jouer_tour(equipe_actuelle, equipe_adverse);
                return;
            }
            utiliser_technique_speciale(combattant_actuel, &combattant_actuel->spe_defense, equipe_adverse, equipe_actuelle); 
            break;
        case 4: 
            if (combattant_actuel->cooldown_vitesse > 0) {
                printf("Technique pas encore prete! (%d tours restants)\n", combattant_actuel->cooldown_vitesse);
                jouer_tour(equipe_actuelle, equipe_adverse);
                return;
            }
            utiliser_technique_speciale(combattant_actuel, &combattant_actuel->spe_vitesse, equipe_adverse, equipe_actuelle); 
            break;
    }
    
    appliquer_effets(combattant_actuel);
    diminuer_cooldowns(combattant_actuel);
}

void afficher_combat(Equipe e1, Equipe e2) {
    printf("\n========================== COMBAT ==========================\n");
    printf("| %-25s | %-25s |\n", e1.Nom_equipe, e2.Nom_equipe);
    printf("|---------------------------|---------------------------|\n");
    
    // Combattant 1
    printf("| 1. %-22s | 1. %-22s |\n", 
           (e1.fighter_1->pv > 0) ? e1.fighter_1->nom : "[K.O.]", 
           (e2.fighter_1->pv > 0) ? e2.fighter_1->nom : "[K.O.]");
    printf("|   PV:%-4d Att:%-3d Def:%-3d |   PV:%-4d Att:%-3d Def:%-3d |\n",
           (e1.fighter_1->pv > 0) ? e1.fighter_1->pv : 0,
           e1.fighter_1->attaque, e1.fighter_1->defense,
           (e2.fighter_1->pv > 0) ? e2.fighter_1->pv : 0,
           e2.fighter_1->attaque, e2.fighter_1->defense);
    
    // Combattant 2
    printf("| 2. %-22s | 2. %-22s |\n", 
           (e1.fighter_2->pv > 0) ? e1.fighter_2->nom : "[K.O.]", 
           (e2.fighter_2->pv > 0) ? e2.fighter_2->nom : "[K.O.]");
    printf("|   PV:%-4d Att:%-3d Def:%-3d |   PV:%-4d Att:%-3d Def:%-3d |\n",
           (e1.fighter_2->pv > 0) ? e1.fighter_2->pv : 0,
           e1.fighter_2->attaque, e1.fighter_2->defense,
           (e2.fighter_2->pv > 0) ? e2.fighter_2->pv : 0,
           e2.fighter_2->attaque, e2.fighter_2->defense);
    
    // Combattant 3
    printf("| 3. %-22s | 3. %-22s |\n", 
           (e1.fighter_3->pv > 0) ? e1.fighter_3->nom : "[K.O.]", 
           (e2.fighter_3->pv > 0) ? e2.fighter_3->nom : "[K.O.]");
    printf("|   PV:%-4d Att:%-3d Def:%-3d |   PV:%-4d Att:%-3d Def:%-3d |\n",
           (e1.fighter_3->pv > 0) ? e1.fighter_3->pv : 0,
           e1.fighter_3->attaque, e1.fighter_3->defense,
           (e2.fighter_3->pv > 0) ? e2.fighter_3->pv : 0,
           e2.fighter_3->attaque, e2.fighter_3->defense);
    
    printf("===========================================================\n");
}

void afficher_details_combattant(Combattant c) {
    printf("\n=== %s ===\n", c.nom);
    printf("PV: %d/%d | Att: %d | Def: %d | Agi: %d | Vit: %d\n", 
           c.pv, c.pv_max, c.attaque, c.defense, c.agilite, c.vitesse);
    
    printf("\nAttaque speciale:\n");
    printf("- %s: %s\n", c.spe_attaque.nom, c.spe_attaque.description);
    printf("  Valeur: %d | Duree: %d tours | Recharge: %d tours | Cible: %s\n",
           c.spe_attaque.valeur, c.spe_attaque.duree, 
           c.spe_attaque.rechargement, c.spe_attaque.type_cible);
    
    printf("\nDefense speciale:\n");
    printf("- %s: %s\n", c.spe_defense.nom, c.spe_defense.description);
    printf("  Valeur: %d | Duree: %d tours | Recharge: %d tours | Cible: %s\n",
           c.spe_defense.valeur, c.spe_defense.duree, 
           c.spe_defense.rechargement, c.spe_defense.type_cible);
    
    printf("\nVitesse speciale:\n");
    printf("- %s: %s\n", c.spe_vitesse.nom, c.spe_vitesse.description);
    printf("  Valeur: %d | Duree: %d tours | Recharge: %d tours | Cible: %s\n",
           c.spe_vitesse.valeur, c.spe_vitesse.duree, 
           c.spe_vitesse.rechargement, c.spe_vitesse.type_cible);
    
    if (c.effets.tours_restants > 0) {
        printf("\nEffets temporaires (%d tours restants):\n", c.effets.tours_restants);
        if (c.effets.attaque_boost != 0) printf("- Attaque +%d\n", c.effets.attaque_boost);
        if (c.effets.defense_boost != 0) printf("- Defense +%d\n", c.effets.defense_boost);
        if (c.effets.vitesse_boost != 0) printf("- Vitesse +%d\n", c.effets.vitesse_boost);
    }
}

void afficher_liste_combattants(Combattant* liste, int nb) {
    printf("\n=== LISTE DES COMBATTANTS (%d) ===\n", nb);
    for (int i = 0; i < nb; i++) {
        printf("\n%d. %s (PV: %d/%d, Att: %d, Def: %d, Agi: %d, Vit: %d)\n",
               i + 1,
               liste[i].nom,
               liste[i].pv, liste[i].pv_max,
               liste[i].attaque,
               liste[i].defense,
               liste[i].agilite,
               liste[i].vitesse);
        
        printf("   Attaque: %s (%s) - Rech: %d tours\n", 
               liste[i].spe_attaque.nom, liste[i].spe_attaque.description,
               liste[i].spe_attaque.rechargement);
        printf("   Defense: %s (%s) - Rech: %d tours\n",
               liste[i].spe_defense.nom, liste[i].spe_defense.description,
               liste[i].spe_defense.rechargement);
        printf("   Vitesse: %s (%s) - Rech: %d tours\n",
               liste[i].spe_vitesse.nom, liste[i].spe_vitesse.description,
               liste[i].spe_vitesse.rechargement);
    }
}

Equipe creer_equipe(Combattant* liste, int nb_combattants, int num_equipe) {
    Equipe e;
    int choix;
    int combattants_choisis[3] = {0};

    printf("\n=== CREATION EQUIPE %d ===\n", num_equipe);
    
    printf("Nom de l'equipe: ");
    scanf(" %99[^\n]", e.Nom_equipe);
    while(getchar() != '\n');

    afficher_liste_combattants(liste, nb_combattants);

    // Allouer de la mémoire pour les combattants de l'équipe
    e.fighter_1 = malloc(sizeof(Combattant));
    e.fighter_2 = malloc(sizeof(Combattant));
    e.fighter_3 = malloc(sizeof(Combattant));

    for (int i = 0; i < 3; i++) {
        do {
            printf("\nChoix du combattant %d (1-%d): ", i + 1, nb_combattants);
            
            if (scanf("%d", &choix) != 1) {
                printf("Entree invalide! Veuillez entrer un nombre.\n");
                while(getchar() != '\n');
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

        // Copier le combattant choisi dans l'équipe
        switch (i) {
            case 0: *e.fighter_1 = liste[choix - 1]; break;
            case 1: *e.fighter_2 = liste[choix - 1]; break;
            case 2: *e.fighter_3 = liste[choix - 1]; break;
        }

        printf("%s ajoute a l'equipe!\n", liste[choix - 1].nom);
    }

    return e;
}

void afficher_equipe(Equipe e) {
    printf("\n=== EQUIPE: %s ===\n", e.Nom_equipe);
    printf("1. %s (PV: %d/%d, Att: %d, Def: %d, Agi: %d, Vit: %d)\n", 
           e.fighter_1->nom, e.fighter_1->pv, e.fighter_1->pv_max, 
           e.fighter_1->attaque, e.fighter_1->defense, e.fighter_1->agilite, e.fighter_1->vitesse);
    printf("2. %s (PV: %d/%d, Att: %d, Def: %d, Agi: %d, Vit: %d)\n", 
           e.fighter_2->nom, e.fighter_2->pv, e.fighter_2->pv_max,
           e.fighter_2->attaque, e.fighter_2->defense, e.fighter_2->agilite, e.fighter_2->vitesse);
    printf("3. %s (PV: %d/%d, Att: %d, Def: %d, Agi: %d, Vit: %d)\n", 
           e.fighter_3->nom, e.fighter_3->pv, e.fighter_3->pv_max,
           e.fighter_3->attaque, e.fighter_3->defense, e.fighter_3->agilite, e.fighter_3->vitesse);
}

void liberer_equipe(Equipe e) {
    free(e.fighter_1);
    free(e.fighter_2);
    free(e.fighter_3);
}

void combat_pvp(Equipe equipe1, Equipe equipe2) {
    printf("\nLe combat commence!\n");
    srand(time(NULL));
    
    int tour = 1;
    int equipe1_ko = 0, equipe2_ko = 0;
    
    while (!equipe1_ko && !equipe2_ko) {
        printf("\n=== TOUR %d ===\n", tour);
        afficher_combat(equipe1, equipe2);
        
        equipe1_ko = verifier_equipe_ko(equipe1);
        equipe2_ko = verifier_equipe_ko(equipe2);
        if (equipe1_ko || equipe2_ko) break;
        
        printf("\n--- Tour de l'equipe %s ---\n", equipe1.Nom_equipe);
        jouer_tour(&equipe1, &equipe2);
        
        equipe2_ko = verifier_equipe_ko(equipe2);
        if (equipe2_ko) break;
        
        printf("\n--- Tour de l'equipe %s ---\n", equipe2.Nom_equipe);
        jouer_tour(&equipe2, &equipe1);
        
        tour++;
    }
    
    if (equipe1_ko) {
        printf("\nL'equipe %s a gagne!\n", equipe2.Nom_equipe);
    } else {
        printf("\nL'equipe %s a gagne!\n", equipe1.Nom_equipe);
    }
    
    liberer_equipe(equipe1);
    liberer_equipe(equipe2);
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

    combat_pvp(equipe1, equipe2);
}

void menu_principal() {
    int choix;
    do {
        printf("\n=== ONE PIECE FIGHT ===\n");
        printf("1. Mode PvP\n");
        printf("2. Mode PvE (non implémenté)\n");
        printf("3. Quitter\n");
        printf("Choix: ");
        
        if (scanf("%d", &choix) != 1) {
            printf("Entree invalide!\n");
            while(getchar() != '\n');
            continue;
        }

        switch (choix) {
            case 1: menu_pvp(); break;
            case 2: printf("Mode PvE non implémenté\n"); break;
            case 3: printf("Au revoir!\n"); exit(0);
            default: printf("Choix invalide!\n");
        }
    } while (1);
}

int main() {
    menu_principal();
    return 0;
}
