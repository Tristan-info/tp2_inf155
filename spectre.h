// spectre.h
#ifndef SPECTRE_H
#define SPECTRE_H

#include "tuiles.h"

// Calcule les intégrales (avec et sans seuil) à partir du tableau de fréquences
// Met à jour les champs .integrale_lumin_compl, .integrale_lumin_seuil, et .seuil_lumin
void calculer_integrales_spectre(t_spectre_gris *sp, double seuil);

// Libère un spectre (pas la tuile, elle est stockée par valeur)
void detruire_spectre(t_spectre_gris *sp);

void afficher_spectre(const t_spectre_gris *sp);

#endif
