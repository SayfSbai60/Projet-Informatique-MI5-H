#include "combat.h"
#include "combattant.h"
#include "utils.h"
#include "menu.h"



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
        afficher_combat(equipe1, equipe2);
        
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


void attaque_de_base_pnj(Combattant* attaquant, Equipe* equipe_adverse) {
   
    Combattant* cible = NULL;
    do {
        switch(rand() % 3) {
            case 0: cible = equipe_adverse->fighter_1; break;
            case 1: cible = equipe_adverse->fighter_2; break;
            case 2: cible = equipe_adverse->fighter_3; break;
        }
    } while (cible == NULL || cible->pv <= 0);

    
    int chance_esquive = 10 + (cible->agilite - attaquant->agilite);
    if (rand() % 100 < chance_esquive) {
        printf("%s esquive l'attaque de %s (Chance: %d%%)!\n", 
              cible->nom, attaquant->nom, chance_esquive);
        return;
    }

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

void combat_pve_simple(Equipe equipe_joueur, Equipe equipe_ia, int difficulte) {
    int tour = 1;
    const int MAX_TOURS = 100;

    printf("\n=== MODE PVE ===\n");
    printf("Difficulte: %s\n", difficulte == 1 ? "Noob" : "Facile");
    
    while (!verifier_equipe_ko(equipe_joueur) && !verifier_equipe_ko(equipe_ia) && tour <= MAX_TOURS) {
        printf("\n--- TOUR %d ---\n", tour++);
        afficher_combat(equipe_joueur, equipe_ia);

        // Tour du joueur
        if (!verifier_equipe_ko(equipe_ia)) {
            printf("\n[VOTRE TOUR]\n");
            jouer_tour(&equipe_joueur, &equipe_ia);
            if (verifier_equipe_ko(equipe_ia)) break;
        }

        // Tour de l'IA
        if (!verifier_equipe_ko(equipe_joueur)) {
            printf("\n[TOUR ENNEMIE]\n");
            
            Combattant* attaquant_ia = choisir_combattant_aleatoire(&equipe_ia);
            if (!attaquant_ia) continue;
            
            Combattant* cible = NULL;
            if (difficulte == 1) { // Noob - cible aléatoire
                cible = choisir_combattant_aleatoire(&equipe_joueur);
            } else { // Facile - cible la plus faible
                cible = trouver_combattant_faible(&equipe_joueur);
            }
            
            attaque_de_base_pnj(attaquant_ia, &equipe_joueur);
            
            if (verifier_equipe_ko(equipe_joueur)) break;
        }
    }
    
    // Résultat du combat
    if(verifier_equipe_ko(equipe_joueur)) {
        printf("L'ennemie a gagne !\n");
    }
    else if(verifier_equipe_ko(equipe_ia)) {
        printf("Vous avez gagne !\n");
    }
    else if(tour > MAX_TOURS) {
        printf("Egalite, nombre de tour maximum atteint !\n");
    }
    
    liberer_equipe(equipe_joueur);
    liberer_equipe(equipe_ia);
    menu_principal();
}

