#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tuiles.h"
#include "recouvrements.h"
#include "spectre.h"
#include "qdbmp.h"

int lire_donnees_fich(FILE *f, char *nom_image, double *seuil, int *nb_col, int *nb_lig, double *prop_garde,
                      double *prop_min) {
    return fscanf(f, "%s %lf %d %d %lf %lf", nom_image, seuil, nb_col, nb_lig, prop_garde, prop_min) == 6;
}


FILE *ouvrir_fich_data(const char *nom_fich, int *nb_tests) {
    if (!nom_fich || !nb_tests) return NULL;

    FILE *f = fopen(nom_fich, "r");
    if (!f) return NULL;

    // Lire le nombre de tests (présumé être sur la première ligne)
    if (fscanf(f, "%d", nb_tests) != 1 || *nb_tests <= 0) {
        fclose(f);
        return NULL;
    }

    return f;  // fichier ouvert, prêt à être lu pour les tests
}






int main(void) {
    BMP *img = BMP_ReadFile("image_test.bmp");

    if (img == NULL) {
        printf("Erreur : impossible de lire l'image.\n");
        return 1;
    }

    //définie les tailles de tuiles (arbitraire)
    int taille1_largeur = 100, taille1_hauteur = 100;
    int taille2_largeur = 250, taille2_hauteur = 100;
    int taille3_largeur = 500, taille3_hauteur = 300;
    int largeur, hauteur;
    for (int i = 1; i <= 3; i++) {
        switch (i) {
            case 1:
                largeur = taille1_largeur;
                hauteur = taille1_hauteur;
                break;
            case 2:
                largeur = taille2_largeur;
                hauteur = taille2_hauteur;
                break;
            case 3:
                largeur = taille3_largeur;
                hauteur = taille3_hauteur;
                break;
        }

        printf("\n=== Test avec tuiles de taille %dx%d ===\n", largeur, hauteur);


        // calculer nombre de tuiles
        int nb_tuiles = get_nb_tuiles(img, largeur, hauteur);
        printf("Nombre de tuiles possibles : %d\n", nb_tuiles);
        if (nb_tuiles == 0)
            continue;

        // initialiser tuile
        t_tuile tuile;
        init_tuile(largeur, hauteur, &tuile);

        //test d'une tuile aléatoire (arbitraire)
        int k = nb_tuiles / 3;
        if (!get_pos_kieme_tuile(img, k, &tuile)) {
            printf("Tuile #%d invalide.\n", k);
            continue;
        }

        printf("Tuile #%d positionnée à (%d, %d)\n", get_id_tuile(&tuile), tuile.offset_col, tuile.offset_lig);

        // créer un spectre de tuile (test erreur d'allocation)
        t_spectre_gris *sp = creer_spectre_tuile(img, &tuile);
        if (sp == NULL) {
            printf("Erreur lors de la création du spectre.\n");
            continue;
        }

        printf("Intégrale complète (seuil 0.0) = %.4f\n", get_integrale_seuil0(sp));

        // SEUIL INTÉGRATION (arbitraire)
        double seuil = 0.6;
        double integrale_seuil = calcul_integrale_seuil_lumin(sp, seuil);
        printf("Intégrale au-dessus du seuil %.2f = %.4f \n", seuil, integrale_seuil);

        // extraction et sauvegarde image
        char nom_couleur[50];
        char nom_gris[50];

        sprintf(nom_couleur, "Tuile%d.bmp", k);
        sprintf(nom_gris, "TuileGris%d.bmp", k);

        BMP *bmp_couleur = get_bitmap_tuile(img, &tuile);
        BMP *bmp_gris = get_bitmap_gris_tuile(img, &tuile, 0.0); // en gris sans seuil

        if (bmp_couleur && bmp_gris) {
            BMP_WriteFile(bmp_couleur, nom_couleur);
            BMP_WriteFile(bmp_gris, nom_gris);
            printf("Images sauvegardées : %s et %s\n", nom_couleur, nom_gris);
        }

        // libérer mémoire en fin de boucle
        if (bmp_couleur) BMP_Free(bmp_couleur);
        if (bmp_gris) BMP_Free(bmp_gris);
        if (sp) free(sp);
    }

    // libérer mémoire de l'image originale
    BMP_Free(img);

    printf("\nTests terminés.\n");

    return 0;
}



    int partie1_main(void) {
        {
            FILE *config = fopen("parametres.txt", "r");
            if (!config) {
                printf("Erreur d'ouverture du fichier de configuration.\n");
                return 1;
            }

            int i = 1;
            while (!feof(config)) {
                char nom_img[100], log_name[100];
                double seuil, prop_garde, prop_min;
                int nb_col, nb_lig;

                if (lire_donnees_fich(config, nom_img, &seuil, &nb_col, &nb_lig, &prop_garde, &prop_min)) {
                    BMP *img = BMP_ReadFile(nom_img);
                    if (!img) {
                        printf("Erreur de lecture de l'image : %s\n", nom_img);
                        continue;
                    }

                    calibrer_taille_tuile(img, &nb_col, &nb_lig);
                    int nb_tuiles = get_nb_tuiles(img, nb_col, nb_lig);
                    t_recouvrement *rec = init_recouvrement(nb_tuiles, nb_col, nb_lig);

                    t_tuile tuile;
                    for (int k = 0; k < nb_tuiles; ++k) {
                        init_tuile(nb_col, nb_lig, &tuile);
                        if (get_pos_kieme_tuile(img, k, &tuile)) {
                            t_spectre_gris *sp = creer_spectre_tuile(img, &tuile);
                            ajouter_spectre_rec(sp, rec);
                        }
                    }

                    calcul_integrales_seuil(rec, seuil);
                    trier_spectres(rec, seuil);

                    sprintf(log_name, "log%d.txt", i);
                    BMP *result = reconstruire_image(img, rec, prop_garde, prop_min, log_name);
                    if (result) {
                        char nom_result[100];
                        sprintf(nom_result, "recouvrement%d.bmp", i);
                        BMP_WriteFile(result, nom_result);
                        BMP_Free(result);
                        printf("\nRecouvrement %d généré : %s\n", i, nom_result);
                    }

                    detruire_recouvrement(rec);
                    BMP_Free(img);
                    i++;
                }
            }

            fclose(config);
            printf("\nTous les recouvrements ont été traités.\n");
            return 0;
        }
    }


