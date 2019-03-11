//
// Created by lecalvmy on 9/13/18.
//

#include "BlackScholesModel.hpp"

/**
* Constructeur de la classe
* @param[in] size : nombre d'actifs du modèle
* @param[in]  r : taux d'intérêt
* @param[in]  rho : paramètre de corrélation
* @param[in]  sigma : vecteur de volatilités
* @param[in]  spot : valeurs initiales des sous-jacents
* @param[in]  trend : tendance du modèle
*/
BlackScholesModel::BlackScholesModel(int size, double r, double rho, PnlVect *sigma, PnlVect *spot, PnlVect *trend){

    size_ = size;
    r_ = r;
    rho_ = rho;
    sigma_ = sigma;
    spot_ = spot;
    trend_ = trend ;
}

/**
     * Génère une trajectoire du modèle et la stocke dans path
     *
     * @param[out] path contient une trajectoire du modèle.
     * C'est une matrice de taille (nbTimeSteps+1) x d
     * @param[in] T  maturité
     * @param[in] nbTimeSteps nombre de dates de constatation
     */
void BlackScholesModel::asset(PnlMat *path, double T, int nbTimeSteps, PnlRng *rng)
{
    double delta_t = T / nbTimeSteps;

    //Remplir la matrice de correlation
    PnlMat *mat_Chol = pnl_mat_create_from_scalar(size_, size_, rho_ );
    for (int i = 0; i < size_; ++i) {
        pnl_mat_set(mat_Chol, i, i, 1);
    }

    //Matrice de Cholesky
    pnl_mat_chol(mat_Chol);

    //Simuler vecteurs gaussiens
    PnlMat *suite_Gauss = pnl_mat_create(nbTimeSteps + 1, size_);
    PnlVect *G = pnl_vect_new();
    for (int d = 0; d < size_; ++d) {
        G = pnl_vect_new();
        pnl_vect_rng_normal(G, nbTimeSteps + 1, rng);
        pnl_mat_set_col(suite_Gauss, G, d);
    }

    //Calcul des prix
    PnlVect *row_Chol = pnl_vect_create(size_);
    PnlVect *row_Gauss = pnl_vect_create(size_);
    double produitScalaire;
    for (int d = 0; d < size_ ; ++d) {
        double prix_Prec = pnl_vect_get(spot_, d);
        double prix = prix_Prec;
        pnl_mat_set(path, 0, d, prix_Prec);
        double sigma = pnl_vect_get(sigma_, d);
        pnl_mat_get_row(row_Chol, mat_Chol, d);
        for (int i = 1; i < nbTimeSteps+1; ++i) {
            pnl_mat_get_row(row_Gauss, suite_Gauss, i);
            produitScalaire = pnl_vect_scalar_prod(row_Gauss, row_Chol);
            prix = prix_Prec * exp((r_-(pow(sigma,2))/2) * delta_t + produitScalaire * sigma * sqrt(delta_t));
            pnl_mat_set(path, i, d, prix);
            prix_Prec = prix;
        }
    }
    pnl_vect_free(&row_Chol);
    pnl_vect_free(&row_Gauss);
    pnl_mat_free(&suite_Gauss);
    pnl_mat_free(&mat_Chol);
    pnl_vect_free(&G);
}

/**
 * Calcule une trajectoire du sous-jacent connaissant le
 * passé jusqu' à la date t
 *
 * @param[out] path  contient une trajectoire du sous-jacent
 * donnée jusqu'à l'instant t par la matrice past
 * @param[in] t date jusqu'à laquelle on connait la trajectoire.
 * t n'est pas forcément une date de discrétisation
 * @param[in] nbTimeSteps nombre de pas de constatation
 * @param[in] T date jusqu'à laquelle on simule la trajectoire
 * @param[in] past trajectoire réalisée jusqu'a la date t
 */
void BlackScholesModel::asset(PnlMat *path, double t, double T, int nbTimeSteps, PnlRng *rng, const PnlMat *past, double nbSteps, int a){

    double delta_t = T / nbTimeSteps;

    //Remplir la mtrice de correlation
    PnlMat *mat_Chol = pnl_mat_create_from_scalar(size_, size_, rho_ );
    for (int i = 0; i < size_; ++i) {
        pnl_mat_set(mat_Chol, i, i, 1);
    }

    //Matrice de Cholesky
    pnl_mat_chol(mat_Chol);

    //nbSteps correspondant
    /*int a = 0;
    double step_1 = t * nbTimeSteps / T ;
    double nbSteps = floor(t * nbTimeSteps / T);
    if (nbSteps == step_1) {
      nbSteps += 1 ;
    }
    else{
      nbSteps += 2 ;
      a = 1;
    }*/

    //simuler les vecteurs gaussien
    PnlMat *suite_Gauss = pnl_mat_create(nbTimeSteps - nbSteps + 2 + a, size_);
    PnlVect *G = pnl_vect_new();
    for (int d = 0; d < size_; ++d) {
        G = pnl_vect_new();
        pnl_vect_rng_normal(G, nbTimeSteps - nbSteps + 2 + a, rng);
        pnl_mat_set_col(suite_Gauss, G, d);
    }
    //Calcul des prix
    PnlVect *row_Chol = pnl_vect_create(size_);
    PnlVect *row_Gauss = pnl_vect_create(size_);
    double produitScalaire;

    //pnl_mat_set_subblock(path, past, 0, 0);

    //Simuler les valeurs suivantes
    for (int d = 0; d < size_ ; ++d) {
        double prix_Prec = pnl_mat_get(past, past->m - 1, d);
        double prix = 0;
        double cours = 1;
        double sigma = pnl_vect_get(sigma_, d);
        pnl_mat_get_row(row_Chol, mat_Chol,d);
        for (int i = 1; i < nbTimeSteps - nbSteps + 2 + a; ++i) {
            pnl_mat_get_row(row_Gauss, suite_Gauss, i);
            produitScalaire = pnl_vect_scalar_prod(row_Gauss, row_Chol);
            cours *= exp((r_-(pow(sigma,2))/2)* delta_t + produitScalaire * sigma * sqrt(delta_t));
            prix = prix_Prec * cours;
            pnl_mat_set(path, (i - 1 + nbSteps - a), d, prix);
        }
    }

    // Free
    pnl_vect_free(&row_Chol);
    pnl_mat_free(&mat_Chol);
    pnl_vect_free(&row_Gauss);
    pnl_mat_free(&suite_Gauss);
    pnl_vect_free(&G);
}

/**
 * Shift d'une trajectoire du sous-jacent
 *
 * @param[in]  path contient en input la trajectoire
 * du sous-jacent
 * @param[out] shift_path contient la trajectoire path
 * dont la composante d a été shiftée par (1+h)
 * à partir de la date t.
 * @param[in] t date à partir de laquelle on shift
 * @param[in] h pas de différences finies
 * @param[in] d indice du sous-jacent à shifter
 * @param[in] timestep pas de constatation du sous-jacent
 */
void BlackScholesModel::shiftAsset(PnlMat *shift_path, const PnlMat *path, int d, double h, double t, double timestep){

    int nbTimeSteps = path->m;

    //nbSteps correspondant
    double step_1 = t/timestep;
    double nbSteps = floor(t/timestep);
    if (nbSteps != step_1) {
      nbSteps += 1 ;
    }

    pnl_mat_clone(shift_path, path);

    for (int i = nbSteps; i < nbTimeSteps; i++) {
      pnl_mat_set(shift_path, i , d, (1+h) * pnl_mat_get(path, i , d));
    }
}

/**
* Simulation du marché (simulation du modèle sous la probabilité historique)
* @param[out] simulatedMarket : contient les valeurs simulées du marché
* @param[in] T : maturité
* @param[in] rng : générateur de nombres aléatoires
*/
void BlackScholesModel::simul_market(PnlMat *simulatedMarket, double T, PnlRng *rng){
  int H =  simulatedMarket->m -1;
  double delta_h = T / H;

  //Remplir la matrice de correlation
  PnlMat *mat_Chol = pnl_mat_create_from_scalar(size_, size_, rho_ );
  for (int i = 0; i < size_; ++i) {
      pnl_mat_set(mat_Chol, i, i, 1);
  }

  //Matrice de Cholesky
  pnl_mat_chol(mat_Chol);

  //Simuler vecteurs gaussiens
  PnlMat *suite_Gauss = pnl_mat_create(H + 1, size_);
  PnlVect *G = pnl_vect_new();
  for (int d = 0; d < size_; ++d) {
      G = pnl_vect_new();
      pnl_vect_rng_normal(G, H + 1, rng);
      pnl_mat_set_col(suite_Gauss, G, d);
  }

  //Calcul des prix
  PnlVect *row_Chol = pnl_vect_create(size_);
  PnlVect *row_Gauss = pnl_vect_create(size_);
  double produitScalaire;
  for (int d = 0; d < size_ ; ++d) {
      double prix_Prec = pnl_vect_get(spot_, d);
      double prix = prix_Prec;
      pnl_mat_set(simulatedMarket, 0, d, prix_Prec);
      double sigma = pnl_vect_get(sigma_, d);
      double trend = pnl_vect_get(trend_, d);
      pnl_mat_get_row(row_Chol, mat_Chol, d);
      for (int i = 1; i < H+1; ++i) {
          pnl_mat_get_row(row_Gauss, suite_Gauss, i);
          produitScalaire = pnl_vect_scalar_prod(row_Gauss, row_Chol);
          prix = prix_Prec * exp((trend-(pow(sigma,2))/2) * delta_h + produitScalaire * sigma * sqrt(delta_h));
          pnl_mat_set(simulatedMarket, i, d, prix);
          prix_Prec = prix;
      }
  }

  pnl_vect_free(&row_Chol);
  pnl_vect_free(&row_Gauss);
  pnl_mat_free(&suite_Gauss);
  pnl_mat_free(&mat_Chol);
  pnl_vect_free(&G);

}
