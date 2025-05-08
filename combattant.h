#ifndef COMBATTANT_H
#define COMBATTANT_H

#define MAX_COMBATTANTS 20
#define MAX_LIGNE 512

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

// Fonctions
Combattant* charger_combattants(unsigned int* nb_combattants);
Equipe creer_equipe(Combattant* liste, int nb_combattants, int num_equipe);
void afficher_equipe(Equipe e);
void afficher_liste_combattants(Combattant* liste, int nb);
void afficher_details_combattant(Combattant c);
void liberer_equipe(Equipe e);
Combattant* choisir_combattant_actif(Equipe* equipe);
Combattant* choisir_combattant_autonome(Equipe* equipe);
Combattant* choisir_cible(Equipe* equipe, const char* type);
Combattant* choisir_cible_autonome(Equipe* equipe, const char* type, int difficulte);

#endif