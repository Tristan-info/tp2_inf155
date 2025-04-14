//
// Created by Rayan Darif on 2025-04-10.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "recouvrements.h"
#include "tuiles.h"
#include "spectre.h"
#include "qdbmp.h"

/* PARTIE DE CHATGPT JAI PAS CAPTE
// Comparateur pour tri avec seuil
static int comparer_integrale_seuil(const void *a, const void *b) {
    t_spectre_gris *sp1 = *(t_spectre_gris **) a;
    t_spectre_gris *sp2 = *(t_spectre_gris **) b;
    return (sp2->integrale_lumin_seuil > sp1->integrale_lumin_seuil) -
           (sp2->integrale_lumin_seuil < sp1->integrale_lumin_seuil);
}

// Comparateur pour tri sans seuil
static int comparer_integrale_compl(const void *a, const void *b) {
    t_spectre_gris *sp1 = *(t_spectre_gris **) a;
    t_spectre_gris *sp2 = *(t_spectre_gris **) b;
    return (sp2->integrale_lumin_compl > sp1->integrale_lumin_compl) -
           (sp2->integrale_lumin_compl < sp1->integrale_lumin_compl);
}*/

// ********************************
// DÉFINITIONS DE FONCTIONS PRIVÉES
// ********************************

/**
 *
 * @param rec pointeur vers recouvrement de spectres
 * @param mode 0 pour tri selon intégrale complète, 1 pour tri selon intégrale seuillee
 */
void trier_spectres_complete(t_recouvrement *rec, int mode) {
    for (int i = 1; i <= rec->taille_tab_spectres; i++) {
        t_spectre_gris* x = rec->tab_spectres[i];

        int j = i;
        if (mode == 0) {
            while (j > 0 && rec->tab_spectres[j-1]->integrale_lumin_compl < x->integrale_lumin_compl) {
                rec->tab_spectres[j] = rec->tab_spectres[j-1];
                j -= 1;
            }
        } else if (mode == 1) {
            while (j > 0 && rec->tab_spectres[j-1]->integrale_lumin_seuil < x->integrale_lumin_seuil) {
                rec->tab_spectres[j] = rec->tab_spectres[j-1];
                j -= 1;
            }
        }
        rec->tab_spectres[j] = x;
    }
}

// **********************************
// DÉFINITIONS DE FONCTIONS PUBLIQUES
// **********************************
// Constructeur
t_recouvrement* init_recouvrement(int nb_tuiles, int nbcol_tuile, int nblig_tuile) {
    t_recouvrement *rec = (t_recouvrement*)malloc(sizeof(t_recouvrement));
    if (rec == NULL) return NULL;

    rec->tab_spectres = calloc(nb_tuiles, sizeof(t_spectre_gris *));
    // NOTE: INITIALISE rec->tab_spectres À 0, DONC DES NULL PTRS
    if (rec->tab_spectres == NULL) {
        free(rec);
        return NULL;
    }

    rec->taille_tab_spectres = nb_tuiles;
    rec->largeur_tuile = nbcol_tuile;
    rec->hauteur_tuile = nblig_tuile;

    return rec;
}


// Destructeur
void detruire_recouvrement(t_recouvrement *rec) {
    if (rec==NULL) return;

    for (int i = 0; i < rec->taille_tab_spectres; i++) {
        if (rec->tab_spectres[i]) {
            detruire_spectre(rec->tab_spectres[i]);
        }
    }

    free(rec->tab_spectres);
    free(rec);
}


// Ajouter un spectre dans la structure
int ajouter_spectre_rec(t_spectre_gris *ptr_sp, t_recouvrement *rec) {
    if (ptr_sp==NULL || rec==NULL) return 0;

    int id = ptr_sp->tuile.id_enum;
    if (id < 0 || id >= rec->taille_tab_spectres) return 0;

    rec->tab_spectres[id] = ptr_sp;
    return 1;
}


// Obtenir le k-ième spectre
t_spectre_gris *get_kieme_ptr_sp(const t_recouvrement *rec, int k) {
    if (rec==NULL || k < 0 || k >= rec->taille_tab_spectres) return NULL;
    for (int i; i < rec->taille_tab_spectres; i++) {
        t_spectre_gris* sp = rec->tab_spectres[i];
        if (sp != NULL || sp->tuile.id_enum == k) {
            return sp;
        }
    }
    //si id n'a pas été trouvée
    return NULL;
}


// Calculer toutes les intégrales avec un seuil donné
void calcul_integrales_seuil(t_recouvrement *rec, double seuil) {
    for (int i = 0; i < rec->taille_tab_spectres; ++i) {
        t_spectre_gris *sp = rec->tab_spectres[i];
        if (sp != NULL && sp->seuil_lumin != seuil) {
            calcul_integrale_seuil_lumin(sp, seuil);
        }
    }
}


// Trier les spectres selon les intégrales
void trier_spectres(t_recouvrement *rec, double seuil) {
    if (rec==NULL) return;
    calcul_integrales_seuil(rec, seuil);
    if (seuil == 0) {
        trier_spectres_complete(rec, 0);
    } else if (seuil > 0 && seuil <= 1) {
        trier_spectres_complete(rec, 1);
    }
}


// Reconstruire l'image
/**
 * A FINIR
 *A FINIR
 *
 *A FINIRA FINIR
 *
 *  A FINIR
 *A FINIR
 *A FINIRA FINIR
 *A FINIR
 *A FINIR
 *A FINIRA FINIR
 *
 *A FINIR
 *A FINIR
 *A FINIR
 *
 *A FINIR
 *A FINIR
 * A FINIR
 */
BMP *reconstruire_image(BMP *original, const t_recouvrement *rec,
                        double prop_garde, double prop_min,
                        char *fichier_log) {

    if (original == NULL || rec == NULL || fichier_log == NULL) {
        return NULL;
    }

    BMP *resultat = BMP_Create(BMP_GetWidth(original), BMP_GetHeight(original), BMP_GetDepth(original));
    if (resultat == NULL) {
        return NULL;
    }

    FILE *log = fopen(fichier_log, "w");
    if (log == NULL) {
        BMP_Free(resultat);  // libérer BMP si le log échoue
        return NULL;
    }

    double seuil = rec->tab_spectres[0]->seuil_lumin;
    fprintf(log, "Seuil: %.2f, Taille: (%d,%d), prop_garde: %.2f, prop_min: %.2f\n",
            seuil, rec->largeur_tuile, rec->hauteur_tuile, prop_garde, prop_min);

    double i0;
    if (seuil == 0.0) {
        i0 = rec->tab_spectres[0]->integrale_lumin_compl;
    } else {
        i0 = rec->tab_spectres[0]->integrale_lumin_seuil;
    }

    int *est_copiee = calloc(rec->taille_tab_spectres, sizeof(int));
    if (est_copiee == NULL) {
        fclose(log);
        BMP_Free(resultat);
        return NULL;
    }

    // Étape 1 : copier tuiles principales
    for (int i = 0; i < rec->taille_tab_spectres; ++i) {
        t_spectre_gris *sp = rec->tab_spectres[i];

        double ix;
        if (seuil == 0.0) {
            ix = sp->integrale_lumin_compl;
        } else {
            ix = sp->integrale_lumin_seuil;
        }

        double R = ix / i0;

        if (R > prop_garde) {
            copier_tuile_a_image(resultat, original, &sp->tuile);
            est_copiee[i] = 1;

            int x, y;
            get_offsets_tuile(&sp->tuile, &x, &y);
            fprintf(log, "%d,%d,%d,%.4f,%.4f\n", sp->tuile.id_enum, x, y, ix, R);
        }
    }

    // Étape 2 : copier tuiles voisines
    for (int i = 0; i < rec->taille_tab_spectres; ++i) {
        if (est_copiee[i] == 1) {
            continue;
        }

        t_spectre_gris *sp = rec->tab_spectres[i];

        double ix;
        if (seuil == 0.0) {
            ix = sp->integrale_lumin_compl;
        } else {
            ix = sp->integrale_lumin_seuil;
        }

        double R = ix / i0;

        if (R <= prop_garde && R > prop_min) {
            for (int j = 0; j < rec->taille_tab_spectres; ++j) {
                if (est_copiee[j] == 1 &&
                    tuiles_voisines(&sp->tuile, &rec->tab_spectres[j]->tuile)) {

                    copier_tuile_a_image(resultat, original, &sp->tuile);
                    est_copiee[i] = 1;

                    int x, y;
                    get_offsets_tuile(&sp->tuile, &x, &y);
                    fprintf(log, "%d,%d,%d,%.4f,%.4f\n", sp->tuile.id_enum, x, y, ix, R);
                    break;
                }
            }
        }
    }

    fclose(log);
    free(est_copiee);
    return resultat;
}
