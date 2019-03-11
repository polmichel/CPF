#pragma once

#include "Option.hpp"
#include "BlackScholesModel.hpp"
#include "pnl/pnl_random.h"
#include <math.h>

/// \brief Méthode de Monte Carlo pour les calculs
class MonteCarlo
{
public:
    /*! pointeur vers le modèle de Black Scholes */
    BlackScholesModel *mod_;
    /*! pointeur sur l'option */
    Option *opt_;
    /*! pointeur sur le générateur de nombres aléatoires */
    PnlRng *rng_;
    /*! pas de h */
    double fdStep_;
    /*! nombre de tirages Monte Carlo */
    size_t nbSamples_;

    /**
    * Constructeur de la classe
    * param[in] mod : pointeur vers le modèle
    * param[in] opt : pointeur sur l'option
    * param[in] rng : pointeur sur le générateur
    * param[in]  fdStep : pas de différence finie
    * param[in]  nbSamples : nombre de tirages Monte Carlo
    */
    MonteCarlo(BlackScholesModel *mod, Option *opt, PnlRng *rng, double fdStep, int nbSamples);

    /**
     * Calcule le prix de l'option à la date 0
     *
     * @param[out] prix valeur de l'estimateur Monte Carlo
     * @param[out] ic largeur de l'intervalle de confiance
     */
    void price(double &prix, double &ic);

    /**
     * Calcule le prix de l'option à la date t
     *
     * @param[in]  past contient la trajectoire du sous-jacent
     * jusqu'à l'instant t
     * @param[in] t date à laquelle le calcul est fait
     * @param[out] prix contient le prix
     * @param[out] ic contient la largeur de l'intervalle
     * de confiance sur le calcul du prix
     */
    void price(const PnlMat *past, double t, double &prix, double &ic);

    /**
     * Calcule le delta de l'option à la date t
     *
     * @param[in] past contient la trajectoire du sous-jacent
     * jusqu'à l'instant t
     * @param[in] t date à laquelle le calcul est fait
     * @param[out] delta contient le vecteur de delta
     * @param[in] conf_delta contient le vecteur d'intervalle de confiance sur le calcul du delta
     */
    void delta(const PnlMat *past, double t, PnlVect *delta, PnlVect *conf_delta);

    /**
     * Calcule le prix et le delta des options à tout instant donné
     *
     * @param[out] listPrice contient le prix des options à differents instants
     * @param[in] matDelta contient le delta des options à differents instants
     * @param[in] marketPrice contient la disposition des trajectoires de marché
     * @param[in] H : nombre de rebalancements
     */
    void PriceDelta(PnlVect *listPrice, PnlMat *matDelta, PnlMat *marketPrice, int H);

    /**
     * Construction du portefeuille de couverture
     * Calcul de l'évolution de la part investie au taux sans risque
     *
     * @param[out] listHedge contient la valeur du portefeuille de couverture à differents instants
     * @param[in] marketPrice contient la disposition des trajectoires de marché
     * @param[in] lastDelta delta dernier instant (a maturite)
     * @param[in] lastPrice delta dernier instant (a maturite)
     */
    void listHedge(PnlVect *listHedge, PnlVect* lastDelta, double& lastPrice, PnlMat *marketPrice);

    /**
    * Profit and Loss
    * Calcul de l'erreur de couverture
    * @param[out]  pnl : erreur de couverture
    * @param[in] marketPrice : disposition des trajectoires de marché
    * @param[in] H : nombre de rebalancements
    */
    void pnl(double& pnl, PnlMat *marketPrice, int H);

};
