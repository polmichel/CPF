#pragma once

#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include <ctime>
#include <math.h>

/// \brief Modèle de Black Scholes
class BlackScholesModel
{
public:
    /*! nombre d'actifs du modèle */
    int size_;
    /*! taux d'intérêt */
    double r_;
    /*!  paramètre de corrélation */
    double rho_;
    /*! vecteur de volatilités */
    PnlVect *sigma_;
    /*!  valeurs initiales des sous-jacents */
    PnlVect *spot_;
    /*! vecteur des tendances */
    PnlVect *trend_;

    /**
    * Constructeur de la classe
    * @param[in] size : nombre d'actifs du modèle
    * @param[in]  r : taux d'intérêt
    * @param[in]  rho : paramètre de corrélation
    * @param[in]  sigma : vecteur de volatilités
    * @param[in]  spot : valeurs initiales des sous-jacents
    * @param[in]  trend : tendance du modèle
    */
    BlackScholesModel(int size, double r, double rho, PnlVect *sigma, PnlVect *spot, PnlVect *trend);

    /**
     * Génère une trajectoire du modèle et la stocke dans path
     *
     * @param[out] path contient une trajectoire du modèle.
     * C'est une matrice de taille (nbTimeSteps+1) x d
     * @param[in] T  maturité
     * @param[in] rng generateur de nombres aleatoires
     * @param[in] nbTimeSteps nombre de dates de constatation
     */
    void asset(PnlMat *path, double T, int nbTimeSteps, PnlRng *rng);

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
     * @param[in] rng generateur de nombres aleatoires
     * @param[in] past trajectoire réalisée jusqu'a la date t
     */
    void asset(PnlMat *path, double t, double T, int nbTimeSteps, PnlRng *rng, const PnlMat *past, double nbSteps, int a);

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
    void shiftAsset(PnlMat *shift_path, const PnlMat *path, int d, double h, double t, double timestep);

    /**
    * Simulation du marché (simulation du modèle sous la probabilité historique)
    * @param[out] simulatedMarket : contient les valeurs simulées du marché
    * @param[in]  T : maturité
    * @param[in] rng : générateur de nombres aléatoires
    */
    void simul_market(PnlMat *simulatedMarket, double T, PnlRng *rng);
};
