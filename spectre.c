#include <stdlib.h>
#include "tuiles.h"
#include "spectre.h"


void detruire_spectre(t_spectre_gris *sp) {
    if (sp != NULL) {
        free(sp); // la tuile est incluse par valeur dans le spectre, donc pas besoin de la libérer séparément
    }
}

void afficher_spectre(const t_spectre_gris *sp) {
    if (!sp) return;

    printf("Spectre de la tuile ID %d :\n", sp->tuile.id_enum);
    for (int i = 0; i < NB_FREQUENCES; i++) {
        if (sp->spectre[i] > 0) {
            printf(" %3d: %d\n", i, sp->spectre[i]);
        }
    }
}
