//
// Created by lecalvmy on 9/18/18.
//

#include "PerformanceOption.hpp"

/**
* Constructeur de la classe
* @param[in] T : maturité
* @param[in] nbTimeSteps : nombre de pas de temps de discrétisation
* @param[in] size : dimension du modèle
* @param[in] weights : poids des actifs
*/
PerformanceOption::PerformanceOption(double T, int nbTimeSteps, int size, PnlVect* weights){
    T_ = T;
    nbTimeSteps_ = nbTimeSteps;
    size_ = size;
    weights_ = weights;
}

/**
 * Calcule la valeur du payoff sur la trajectoire
 *
 * @param[in] path est une matrice de taille (N+1) x d
 * contenant une trajectoire du modèle telle que créée
 * par la fonction asset.
 * @return phi(trajectoire)
 */
double PerformanceOption::payoff(const PnlMat *path) {
    double sum_N = 0;
    PnlVect *price_i = pnl_vect_create(size_);
    PnlVect *price_i1 = pnl_vect_create(size_);
    for (int i = 1; i < path->m ; ++i) {
      pnl_mat_get_row(price_i, path, i);
      pnl_mat_get_row(price_i1, path, i-1);
      double rapport = pnl_vect_scalar_prod(weights_, price_i)/pnl_vect_scalar_prod(weights_, price_i1) ;
      sum_N += fmax(rapport - 1, 0);
    }
    pnl_vect_free(&price_i);
    pnl_vect_free(&price_i1);
    return 1 + sum_N;
}
