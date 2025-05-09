#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "combattant.h"

void afficher_separateur(int longueur);
void afficher_ligne_vide(int longueur);
void afficher_ligne_texte(int longueur, const char* texte);
void afficher_combat(Equipe e1, Equipe e2);

#endif
