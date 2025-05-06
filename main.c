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
    int agilite_boost;
    int tours_restants;
} EffetsTemporaires;

typedef struct {
    char nom[50];
    int pv;
    int pv_max;
    int attaque;
    int defense;
    int agilite;
    TechniqueSpeciale spe_attaque;
    TechniqueSpeciale spe_defense;
    TechniqueSpeciale spe_agilite;
    EffetsTemporaires effets;
    int cooldown_attaque;
    int cooldown_defense;
    int cooldown_agilite;
} Combattant;

typedef struct {
    char Nom_equipe[100];
    Combattant* fighter_1;
    Combattant* fighter_2;
    Combattant* fighter_3;
} Equipe;

#define MAX_LIGNE 512
#define MAX_COMBATTANTS 20

Combattant* charger_combattants(unsigned int* nb_combattants) {
    FILE* fichier = fopen("combattants.txt", "r");
    if (!fichier) {
        perror("Erreur ouverture fichier");
        exit(EXIT_FAILURE);
    }

    Combattant* liste = malloc(MAX_COMBATTANTS * sizeof(Combattant));
    if (!liste) {
        perror("Erreur allocation mémoire");
        fclose(fichier);
        exit(EXIT_FAILURE);
    }

    *nb_combattants = 0;
    char ligne[MAX_LIGNE];

    while (fgets(ligne, sizeof(ligne), fichier) && *nb_combattants < MAX_COMBATTANTS) {
        Combattant* c = &liste[*nb_combattants];

        int lus = sscanf(ligne,
            "%49[^;];%d;%d;%d;%d;%d;%49[^;];%99[^;];%d;%d;%d;%9[^;];"
            "%49[^;];%99[^;];%d;%d;%d;%9[^;];"
            "%49[^;];%99[^;];%d;%d;%d;%9[^\n]",
            c->nom,
            &c->pv, &c->pv_max, &c->attaque, &c->defense, &c->agilite,
            c->spe_attaque.nom, c->spe_attaque.description, &c->spe_attaque.valeur,
            &c->spe_attaque.duree, &c->spe_attaque.rechargement, c->spe_attaque.type_cible,
            c->spe_defense.nom, c->spe_defense.description, &c->spe_defense.valeur,
            &c->spe_defense.duree, &c->spe_defense.rechargement, c->spe_defense.type_cible,
            c->spe_agilite.nom, c->spe_agilite.description, &c->spe_agilite.valeur,
            &c->spe_agilite.duree, &c->spe_agilite.rechargement, c->spe_agilite.type_cible
        );

        if (lus == 24) {
            // Réinitialisation des états
            c->cooldown_attaque = c->cooldown_defense = c->cooldown_agilite = 0;
            c->effets.attaque_boost = c->effets.defense_boost = c->effets.agilite_boost = 0;
            c->effets.tours_restants = 0;
            (*nb_combattants)++;
        } else {
            fprintf(stderr, "Ligne mal formatée (%d champs): %s", lus, ligne);
        }
    }

    fclose(fichier);
    return liste;
}

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


void appliquer_effets(Combattant* c) {
    if (c->effets.tours_restants > 0) {
        c->effets.tours_restants--;
        if (c->effets.tours_restants == 0) {
            c->attaque -= c->effets.attaque_boost;
            c->defense -= c->effets.defense_boost;
            c->agilite -= c->effets.agilite_boost;
            c->effets.attaque_boost = 0;
            c->effets.defense_boost = 0;
            c->effets.agilite_boost = 0;
            printf("%s: Les effets temporaires ont expire.\n", c->nom);
        }
    }
}

void diminuer_cooldowns(Combattant* c) {
    if (c->cooldown_attaque > 0) c->cooldown_attaque--;
    if (c->cooldown_defense > 0) c->cooldown_defense--;
    if (c->cooldown_agilite > 0) c->cooldown_agilite--;
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
    char input[10];
    do {
        printf("Choisissez un combattant (1-3): ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Erreur de lecture.\n");
            continue;
        }
        if (sscanf(input, "%d", &choix) != 1) {
            printf("Entree invalide! Veuillez entrer un nombre.\n");
            continue;
        }
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
    char input[10];
    do {
        printf("Choisissez une cible (1-3): ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Erreur de lecture.\n");
            continue;
        }
        if (sscanf(input, "%d", &choix) != 1) {
            printf("Entree invalide! Veuillez entrer un nombre.\n");
            continue;
        }
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
Combattant* choisir_combattant_autonome(Equipe* equipe) {
    Combattant* meilleur = NULL;
    for (int i = 0; i < 3; i++) {
        Combattant* c = (i == 0) ? equipe->fighter_1 : 
                       (i == 1) ? equipe->fighter_2 : 
                       equipe->fighter_3;
        if (c->pv > 0 && (!meilleur || c->pv > meilleur->pv)) {
            meilleur = c;
        }
    }
    return meilleur;
}

Combattant* choisir_cible_autonome(Equipe* equipe, const char* type, int difficulte) {
    Combattant* cible = NULL;
    for (int i = 0; i < 3; i++) {
        Combattant* c = (i == 0) ? equipe->fighter_1 : 
                       (i == 1) ? equipe->fighter_2 : 
                       equipe->fighter_3;
        if (c->pv > 0) {
            if (difficulte == 1) { // NOOB : aléatoire
                if (rand() % 3 == 0) return c;
            }
            if (!cible || c->pv < cible->pv) { // FACILE/MOYEN : plus faible
                cible = c;
            }
        }
    }
    return cible;
}

void attaque_de_base(Combattant* attaquant, Equipe* equipe_adverse) {
    Combattant* cible = choisir_cible(equipe_adverse, "ennemie");
    
    // Calcul de l'esquive basée sur l'agilité (10% de base + différence d'agilité)
    int chance_esquive = 10 + (cible->agilite - attaquant->agilite);
    if (chance_esquive < 5) chance_esquive = 5;  // Minimum 5% de chance
    if (chance_esquive > 40) chance_esquive = 40; // Maximum 40% de chance
    
    if (rand() % 100 < chance_esquive) {
        printf("%s esquive l'attaque grace a son agilite (%d%% chance)!\n", cible->nom, chance_esquive);
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
    if (strstr(tech->description, "Augmente") || strstr(tech->description, "Renforce") || 
        strstr(tech->description, "Protection") || strstr(tech->description, "Resistance") ||
        strstr(tech->description, "Acceleration") || strstr(tech->description, "Déplacement") ||
        strstr(tech->description, "Esquive") || strstr(tech->description, "Contre-attaque") ||
        strstr(tech->description, "Barrière") || strstr(tech->description, "Endurance")) {
        // Effet de boost positif
        if (strstr(tech->description, "attaque") || strstr(tech->description, "Attaque")) {
            cible->attaque += tech->valeur;
            cible->effets.attaque_boost = tech->valeur;
            printf("%s: Attaque augmentee de %d pour %d tours!\n", 
                   cible->nom, tech->valeur, tech->duree);
        }
        else if (strstr(tech->description, "defense") || strstr(tech->description, "Defense") ||
                 strstr(tech->description, "Protection") || strstr(tech->description, "Resistance") ||
                 strstr(tech->description, "Barrière") || strstr(tech->description, "Endurance")) {
            cible->defense += tech->valeur;
            cible->effets.defense_boost = tech->valeur;
            printf("%s: Defense augmentee de %d pour %d tours!\n", 
                   cible->nom, tech->valeur, tech->duree);
        }
        else if (strstr(tech->description, "agilite") || strstr(tech->description, "Agilite") ||
                 strstr(tech->description, "Acceleration") || strstr(tech->description, "Déplacement") ||
                 strstr(tech->description, "Esquive")) {
            cible->agilite += tech->valeur;
            cible->effets.agilite_boost = tech->valeur;
            printf("%s: Agilite augmentee de %d pour %d tours!\n", 
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
        // Dégâts normaux (uniquement pour les techniques d'attaque)
        if (strcmp(tech->type_cible, "ennemi") == 0) {
            cible->pv -= tech->valeur;
            printf("%s utilise %s sur %s et inflige %d degats!\n", 
                   utilisateur->nom, tech->nom, cible->nom, tech->valeur);
            if (cible->pv <= 0) {
                cible->pv = 0;
                printf("%s est K.O.!\n", cible->nom);
            }
        } else {
            printf("%s utilise %s sur %s pour un effet bénéfique!\n",
                   utilisateur->nom, tech->nom, cible->nom);
        }
    }
}

void utiliser_technique_speciale(Combattant* utilisateur, TechniqueSpeciale* tech, Equipe* equipe_adverse, Equipe* equipe_alliee) {
    if (tech == &utilisateur->spe_attaque) {
        Combattant* cible = choisir_cible(equipe_adverse, "ennemie");
        appliquer_technique(utilisateur, tech, cible);
    }
    else {
        Combattant* cible = choisir_cible(equipe_alliee, "alliee");
        appliquer_technique(utilisateur, tech, cible);
    }
    
    // Mettre en rechargement
    if (tech == &utilisateur->spe_attaque) {
        utilisateur->cooldown_attaque = tech->rechargement;
    } else if (tech == &utilisateur->spe_defense) {
        utilisateur->cooldown_defense = tech->rechargement;
    } else if (tech == &utilisateur->spe_agilite) {
        utilisateur->cooldown_agilite = tech->rechargement;
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
    printf("4. Technique speciale - Agilite (%s)", combattant_actuel->spe_agilite.nom);
    if (combattant_actuel->cooldown_agilite > 0) printf(" (Recharge: %d)", combattant_actuel->cooldown_agilite);
    printf("\n");
    
    int choix_action;
    char input[10];
    do {
        printf("Choisissez une action (1-4): ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Erreur de lecture.\n");
            continue;
        }
        if (sscanf(input, "%d", &choix_action) != 1) {
            printf("Entree invalide! Veuillez entrer un nombre.\n");
            continue;
        }
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
            if (combattant_actuel->cooldown_agilite > 0) {
                printf("Technique pas encore prete! (%d tours restants)\n", combattant_actuel->cooldown_agilite);
                jouer_tour(equipe_actuelle, equipe_adverse);
                return;
            }
            utiliser_technique_speciale(combattant_actuel, &combattant_actuel->spe_agilite, equipe_adverse, equipe_actuelle); 
            break;
    }
    
    appliquer_effets(combattant_actuel);
    diminuer_cooldowns(combattant_actuel);
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


void afficher_details_combattant(Combattant c) {
    const int largeur = 60;
    
    printf("\n");
    afficher_separateur(largeur);
    printf("| %-56s |\n", c.nom);
    afficher_separateur(largeur);
    printf("| PV: %-4d/%d | Att: %-3d | Def: %-3d | Agi: %-3d |\n",
           c.pv, c.pv_max, c.attaque, c.defense, c.agilite);
    afficher_separateur(largeur);
    
    // Attaque spéciale
    printf("| %-56s |\n", "Attaque spéciale:");
    printf("| %-56s |\n", c.spe_attaque.nom);
    printf("| %-56s |\n", c.spe_attaque.description);
    printf("| Valeur: %-4d | Durée: %-2d tours | Recharge: %-2d tours |\n",
           c.spe_attaque.valeur, c.spe_attaque.duree, c.spe_attaque.rechargement);
    afficher_separateur(largeur);
    
    // Défense spéciale
    printf("| %-56s |\n", "Défense spéciale:");
    printf("| %-56s |\n", c.spe_defense.nom);
    printf("| %-56s |\n", c.spe_defense.description);
    printf("| Valeur: %-4d | Durée: %-2d tours | Recharge: %-2d tours |\n",
           c.spe_defense.valeur, c.spe_defense.duree, c.spe_defense.rechargement);
    afficher_separateur(largeur);
    
    // Agilité spéciale
    printf("| %-56s |\n", "Agilité spéciale:");
    printf("| %-56s |\n", c.spe_agilite.nom);
    printf("| %-56s |\n", c.spe_agilite.description);
    printf("| Valeur: %-4d | Durée: %-2d tours | Recharge: %-2d tours |\n",
           c.spe_agilite.valeur, c.spe_agilite.duree, c.spe_agilite.rechargement);
    
    if (c.effets.tours_restants > 0) {
        afficher_separateur(largeur);
        printf("| Effets temporaires (%d tours restants):%-25s |\n", 
               c.effets.tours_restants, "");
        if (c.effets.attaque_boost != 0) 
            printf("| - Attaque +%-50d |\n", c.effets.attaque_boost);
        if (c.effets.defense_boost != 0) 
            printf("| - Defense +%-49d |\n", c.effets.defense_boost);
        if (c.effets.agilite_boost != 0) 
            printf("| - Agilité +%-49d |\n", c.effets.agilite_boost);
    }
    
    afficher_separateur(largeur);
}

void afficher_liste_combattants(Combattant* liste, int nb) {
    const int largeur = 80;
    
    printf("\n");
    afficher_separateur(largeur);
    printf("| LISTE DES COMBATTANTS (%d)%-55s |\n", nb, "");
    afficher_separateur(largeur);
    
    for (int i = 0; i < nb; i++) {
        printf("| %2d. %-20s PV: %4d Att: %3d Def: %3d Agi: %3d |\n",
               i+1, liste[i].nom, liste[i].pv, 
               liste[i].attaque, liste[i].defense, liste[i].agilite);
        
        if ((i+1) % 5 == 0 || i == nb-1) {
            afficher_separateur(largeur);
        }
    }
}

Equipe creer_equipe(Combattant* liste, int nb_combattants, int num_equipe) {
    Equipe e;
    int choix;
    char input[10];
    const int largeur = 70;

    printf("\n");
    afficher_separateur(largeur);
    printf("| CREATION EQUIPE %-51d |\n", num_equipe);
    afficher_separateur(largeur);
    
    printf("| Nom de l'equipe: %-49s |\n", "");
    printf("| ");
    if (fgets(e.Nom_equipe, sizeof(e.Nom_equipe), stdin)) {
        e.Nom_equipe[strcspn(e.Nom_equipe, "\n")] = '\0';
    } else {
        strcpy(e.Nom_equipe, "Equipe sans nom");
    }
    printf("\033[A\033[18C%s\n", e.Nom_equipe);

    afficher_liste_combattants(liste, nb_combattants);

    e.fighter_1 = malloc(sizeof(Combattant));
    e.fighter_2 = malloc(sizeof(Combattant));
    e.fighter_3 = malloc(sizeof(Combattant));

    for (int i = 0; i < 3; i++) {
        do {
            printf("\n");
            afficher_separateur(largeur);
            printf("| Selectionnez le combattant %d/3 (1-%d):%-30s |\n", 
                  i+1, nb_combattants, "");
            afficher_separateur(largeur);
            printf("| Choix: %-60s |\n", "");
            
            // Positionnement curseur
            printf("\033[A\033[8C");
            
            if (fgets(input, sizeof(input), stdin) && sscanf(input, "%d", &choix) == 1) {
                if (choix >= 1 && choix <= nb_combattants) {
                    // Vérifier si déjà choisi...
                    printf("| %-60s |\n", liste[choix-1].nom);
                    break;
                }
            }
            printf("Choix invalide!\n");
        } while (1);

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
    const int largeur = 70;
    
    printf("\n");
    afficher_separateur(largeur);
    printf("| EQUIPE: %-59s |\n", e.Nom_equipe);
    afficher_separateur(largeur);
    
    printf("| 1. %-20s PV: %4d Att: %3d Def: %3d Agi: %3d |\n",
           e.fighter_1->nom, e.fighter_1->pv, 
           e.fighter_1->attaque, e.fighter_1->defense, e.fighter_1->agilite);
    
    printf("| 2. %-20s PV: %4d Att: %3d Def: %3d Agi: %3d |\n",
           e.fighter_2->nom, e.fighter_2->pv,
           e.fighter_2->attaque, e.fighter_2->defense, e.fighter_2->agilite);
    
    printf("| 3. %-20s PV: %4d Att: %3d Def: %3d Agi: %3d |\n",
           e.fighter_3->nom, e.fighter_3->pv,
           e.fighter_3->attaque, e.fighter_3->defense, e.fighter_3->agilite);
    
    afficher_separateur(largeur);
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

    const int largeur = 60;
    
    printf("\n");
    afficher_separateur(largeur);
    printf("| %-58s |\n", "MODE PVP");
    afficher_separateur(largeur);
    
    printf("| %-58s |\n", "Creation de l'equipe 1");
    afficher_separateur(largeur);
    Equipe equipe1 = creer_equipe(liste, nb_combattants, 1);
    
    printf("\n");
    afficher_separateur(largeur);
    printf("| %-58s |\n", "Creation de l'equipe 2");
    afficher_separateur(largeur);
    Equipe equipe2 = creer_equipe(liste, nb_combattants, 2);

    printf("\n");
    afficher_separateur(largeur);
    printf("| %-58s |\n", "Equipes pretes !");
    afficher_separateur(largeur);
    afficher_equipe(equipe1);
    afficher_equipe(equipe2);

    free(liste);
    combat_pvp(equipe1, equipe2);
}

int choisir_difficulte() {
    const int largeur = 50;
    int choix;
    char input[10];
    
    do {
        printf("\n");
        afficher_separateur(largeur);
        printf("| %-48s |\n", "CHOIX DE LA DIFFICULTE");
        afficher_separateur(largeur);
        printf("| %-48s |\n", "1. Noob (facile)");
        printf("| %-48s |\n", "2. Facile");
        printf("| %-48s |\n", "3. Moyen");
        afficher_separateur(largeur);
        printf("| Choix: %-40s |\n", "");
        
        // Positionnement curseur
        printf("\033[A\033[9C");
        
        if (fgets(input, sizeof(input), stdin) && sscanf(input, "%d", &choix) == 1) {
            if (choix >= 1 && choix <= 3) break;
        }
        printf("Choix invalide! (1-3)\n");
    } while (1);
    
    return choix;
}

void log_action_ia(Combattant* actif, const char* action, Combattant* cible) {
    printf("[IA] %s %s → Cible: %s (PV: %d)\n", 
           actif->nom, action, cible->nom, cible->pv);
}

void ia_appliquer_technique(Combattant* utilisateur, TechniqueSpeciale* tech, Combattant* cible) {
    printf("%s utilise %s sur %s\n", utilisateur->nom, tech->nom, cible->nom);
    
    if (strstr(tech->description, "Soin") || strstr(tech->description, "soin")) {
        cible->pv = (cible->pv + tech->valeur > cible->pv_max) ? cible->pv_max : cible->pv + tech->valeur;
    } 
    else if (strcmp(tech->type_cible, "ennemi") == 0) {
        cible->pv -= tech->valeur;
        if (cible->pv < 0) cible->pv = 0;
    } 
    else { // Buffs
        if (strstr(tech->description, "attaque")) cible->attaque += tech->valeur;
        if (strstr(tech->description, "defense")) cible->defense += tech->valeur;
        if (strstr(tech->description, "agilite")) cible->agilite += tech->valeur;
    }
}

void ia_attaque_de_base(Combattant* attaquant, Equipe* equipe_adverse) {
    Combattant* cible = choisir_cible_autonome(equipe_adverse, "ennemi", 2); // Mode FACILE par défaut
    int degats = attaquant->attaque - (cible->defense / 2);
    if (degats < 1) degats = 1;
    
    cible->pv -= degats;
    printf("%s attaque %s (%d PV restants)\n", attaquant->nom, cible->nom, cible->pv);
}

void combat_autonome(Equipe equipe_joueur, Equipe equipe_ia, int difficulte) {
    int tour = 1;
    while (!verifier_equipe_ko(equipe_joueur) && !verifier_equipe_ko(equipe_ia)) {
        printf("\n=== TOUR %d ===\n", tour++);
        afficher_combat(equipe_joueur, equipe_ia);

        // Tour joueur (manuel)
        printf("\n--- VOTRE TOUR ---\n");
        jouer_tour(&equipe_joueur, &equipe_ia);
        if (verifier_equipe_ko(equipe_ia)) break;

        // Tour IA (100% autonome)
        printf("\n--- TOUR IA ---\n");
        jouer_ia_autonome(&equipe_ia, &equipe_joueur, difficulte);
    }

    printf(verifier_equipe_ko(equipe_joueur) ? "\nL'IA a gagne!\n" : "\nVous avez gagne!\n");
    liberer_equipe(equipe_joueur);
    liberer_equipe(equipe_ia);
}
void jouer_ia_autonome(Equipe* equipe_ia, Equipe* equipe_joueur, int difficulte){
    Combattant* actif = choisir_combattant_autonome(equipe_ia);
    if (!actif) return;

    printf("\nIA (%s) utilise %s:\n", 
           (difficulte == 1) ? "NOOB" : (difficulte == 2) ? "FACILE" : "MOYEN",
           actif->nom);

    // NOOB : seulement attaque de base aléatoire
    if (difficulte == 1) {
        ia_attaque_de_base(actif, equipe_joueur);  // <-- Utilisation de la version IA
        return;
    }

    // FACILE/MOYEN : logique optimisée
    TechniqueSpeciale* tech = NULL;
    Combattant* cible = NULL;

    // 1. Priorité aux attaques spéciales si disponibles
    if (actif->cooldown_attaque == 0) {
        tech = &actif->spe_attaque;
        cible = choisir_cible_autonome(equipe_joueur, "ennemi", difficulte);
    } 
    // 2. Soins/protections sur alliés faibles
    else if (actif->cooldown_defense == 0 && 
             (strstr(actif->spe_defense.description, "Soin") || 
              strstr(actif->spe_defense.description, "Protection"))) {
        tech = &actif->spe_defense;
        cible = choisir_cible_autonome(equipe_ia, "allie", difficulte);
    }
    // 3. Buffs d'agilité
    else if (actif->cooldown_agilite == 0) {
        tech = &actif->spe_agilite;
        cible = choisir_cible_autonome(equipe_ia, "allie", difficulte);
    }

    // Application de l'action choisie
    if (tech && cible) {
        ia_appliquer_technique(actif, tech, cible);  // <-- Utilisation de la version IA
        if (tech == &actif->spe_attaque) actif->cooldown_attaque = tech->rechargement;
        else if (tech == &actif->spe_defense) actif->cooldown_defense = tech->rechargement;
        else if (tech == &actif->spe_agilite) actif->cooldown_agilite = tech->rechargement;
    } else {
        // Fallback : attaque de base
        ia_attaque_de_base(actif, equipe_joueur);  // <-- Utilisation de la version IA
    }

    appliquer_effets(actif);
    diminuer_cooldowns(actif);
}


void menu_pve() {
    unsigned int nb_combattants;
    Combattant* liste = charger_combattants(&nb_combattants);
    const int largeur = 60;

    printf("\n");
    afficher_separateur(largeur);
    printf("| %-58s |\n", "MODE PVE");
    afficher_separateur(largeur);
    
    int difficulte = choisir_difficulte();
    
    printf("\n");
    afficher_separateur(largeur);
    printf("| %-58s |\n", "Creation de votre equipe");
    afficher_separateur(largeur);
    Equipe equipe_joueur = creer_equipe(liste, nb_combattants, 1);
    
    printf("\n");
    afficher_separateur(largeur);
    printf("| %-58s |\n", "Generation de l'equipe IA");
    afficher_separateur(largeur);
    
    // Génération équipe IA...
    Equipe equipe_ia;
    strcpy(equipe_ia.Nom_equipe, "IA Enemy");
    equipe_ia.fighter_1 = malloc(sizeof(Combattant));
    equipe_ia.fighter_2 = malloc(sizeof(Combattant));
    equipe_ia.fighter_3 = malloc(sizeof(Combattant));
    
    // Choix aléatoire des combattants IA (excluant ceux du joueur)
    for (int i = 0; i < 3; i++) {
        int index;
        do {
            index = rand() % nb_combattants;
        } while (index == (equipe_joueur.fighter_1 - liste) || 
                 index == (equipe_joueur.fighter_2 - liste) || 
                 index == (equipe_joueur.fighter_3 - liste));
        
        *(i == 0 ? equipe_ia.fighter_1 : i == 1 ? equipe_ia.fighter_2 : equipe_ia.fighter_3) = liste[index];
    }

    printf("\n=== ÉQUIPE IA GENEREE AUTOMATIQUEMENT ===\n");
    afficher_equipe(equipe_ia);
    
    free(liste);
    combat_autonome(equipe_joueur, equipe_ia, difficulte);
}

void menu_principal() {
    const int largeur = 40;
    int choix;
    char input[10];
    
    do {
        printf("\n");
        afficher_separateur(largeur);
        printf("| %-38s |\n", "ONE PIECE FIGHT");
        afficher_separateur(largeur);
        printf("| %-38s |\n", "1. Mode PvP (Joueur vs Joueur)");
        printf("| %-38s |\n", "2. Mode PvE (Joueur vs IA)");
        printf("| %-38s |\n", "3. Quitter");
        afficher_separateur(largeur);
        printf("| Choix: %-30s |\n", "");
        
        // Positionner le curseur pour le choix
        printf("\033[A"); // Remonter d'une ligne
        printf("\033[9C"); // Avancer de 9 caractères
        
        if (fgets(input, sizeof(input), stdin)){
            sscanf(input, "%d", &choix);
        } else {
            choix = 0;
        }

        switch (choix) {
            case 1: menu_pvp(); break;
            case 2: menu_pve(); break;
            case 3: 
                printf("\nMerci d'avoir joué !\n");
                exit(0);
            default: 
                printf("Choix invalide!\n");
        }
    } while (1);
}

int main(){

    menu_principal();
    return 0;
}
