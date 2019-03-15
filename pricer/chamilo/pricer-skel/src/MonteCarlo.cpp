//
// Created by lecalvmy on 9/13/18.
//

#include "MonteCarlo.hpp"
#include "mpi.h"
#include <iostream>
#include <string>

using namespace std;

/**
* Constructeur de la classe
* param[in] mod : pointeur vers le modèle
* param[in] opt : pointeur sur l'option
* param[in] rng : pointeur sur le générateur
* param[in] fdStep : pas de différence finie
* param[in] nbSamples : nombre de tirages Monte Carlo
*/
MonteCarlo::MonteCarlo(BlackScholesModel *mod, Option *opt, PnlRng *rng, double fdStep, int nbSamples){
    mod_ = mod;
    opt_ = opt;
    rng_ = rng;
    fdStep_ = fdStep;
    nbSamples_ = nbSamples;
}
/**
    * Calcule le prix de l'option à la date 0
    * @param[out] prix valeur de l'estimateur Monte Carlo
    * @param[out] ic largeur de l'intervalle de confiance
*/
/*void MonteCarlo::price(double &prix, double &ic){

}*/

void MonteCarlo::price_master(double &prix, double &ic, int size){


    double sumPayoff, payoff, esp_carre;
    double priceNumber_i, icNumber_i;
    PnlMat *path;
    path = pnl_mat_create(opt_->nbTimeSteps_ + 1, mod_->size_);

    PnlVect* vectPrices = pnl_vect_create(size);
    PnlVect* vectIcs = pnl_vect_create(size);

    for (int i = 1; i< this->nbSamples_/size; i++) {

        mod_->asset(path, opt_->T_, opt_->nbTimeSteps_, rng_);
        payoff = opt_->payoff(path);
        sumPayoff += payoff;
        esp_carre += payoff*payoff;

    }

    LET(vectPrices, 0) =sumPayoff;
    LET(vectIcs, 0) = esp_carre;

    for (int i = 1; i< size; i++) {

        // réception réponse i
        MPI_Recv(&priceNumber_i, 1, MPI_DOUBLE, i,i, MPI_COMM_WORLD, NULL);
        MPI_Recv(&icNumber_i, 1, MPI_DOUBLE, i,i+size, MPI_COMM_WORLD, NULL);

        // enregistrement réponse i à la bonne zone mémoire grâce aux tags

        LET(vectPrices, i) = priceNumber_i;
        LET(vectIcs, i) = icNumber_i;
    }

    //regroupement des différents prix pour faire la moyenne
    prix = pnl_vect_sum(vectPrices);


    prix/=nbSamples_;


    esp_carre = pnl_vect_sum(vectIcs);
    esp_carre/=nbSamples_;

    double estimateur_carre = exp(-2*mod_->r_*opt_->T_)*(esp_carre/nbSamples_-pow(prix/nbSamples_,2));

    prix *= exp(-mod_->r_*opt_->T_);

    ic = 1.96 * sqrt(estimateur_carre);
}

void MonteCarlo::price_slave(double &prix, double &ic, int size, int rank){
    //envoyer la réponse j au maître
    double sumPayoff, payoff, var;
    PnlMat *path;
    path = pnl_mat_create(opt_->nbTimeSteps_ + 1, mod_->size_);
    for (int i = 1; i< this->nbSamples_/size; i++) {

        mod_->asset(path, opt_->T_, opt_->nbTimeSteps_, rng_);
        payoff = opt_->payoff(path);
        sumPayoff += payoff;
        var += payoff*payoff;

    }
    //rank et rank + size servent à distinguer les réponses en fonction d'un tag unique
    MPI_Send(&sumPayoff, 1, MPI_DOUBLE, 0, rank, MPI_COMM_WORLD);
    MPI_Send(&var, 1, MPI_DOUBLE, 0, rank+size, MPI_COMM_WORLD);
    //MPI_Send(&payoff, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD); //tag à changer deuxième zéro ?
    pnl_mat_free(&path);
}


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
void MonteCarlo::price(const PnlMat *past, double t, double &prix, double &ic){
    double payoff;
    prix = 0;
    PnlMat *path;
    double esp_carre = 0;
    path = pnl_mat_create(opt_->nbTimeSteps_ + 1, mod_->size_);
    pnl_mat_set_subblock(path, past, 0, 0);
    int a = 0;
    double step_1 = t * opt_->nbTimeSteps_ / opt_->T_ ;
    double nbSteps = floor(t * opt_->nbTimeSteps_ / opt_->T_);
    if (nbSteps == step_1) {
      nbSteps += 1 ;
    }
    else{
      nbSteps += 2 ;
      a = 1;
    }
    for (size_t i = 0; i < nbSamples_; ++i) {
        mod_->asset(path, t, opt_->T_, opt_->nbTimeSteps_, rng_, past, nbSteps, a);
        payoff = opt_->payoff(path);
        prix += payoff;
        esp_carre += pow(payoff,2);
    }
    double estimateur_carre = exp(-2*mod_->r_*(opt_->T_- t))*(esp_carre/nbSamples_-pow(prix/nbSamples_,2));
    prix *= exp(-mod_->r_*(opt_->T_ - t))/nbSamples_;
    ic = 1.96 * sqrt(estimateur_carre/nbSamples_);
    pnl_mat_free(&path);
}


/**
 * Calcule le delta de l'option à la date t
 *
 * @param[in] past contient la trajectoire du sous-jacent
 * jusqu'à l'instant t
 * @param[in] t date à laquelle le calcul est fait
 * @param[out] delta contient le vecteur de delta
 * @param[in] conf_delta contient le vecteur d'intervalle de confiance sur le calcul du delta
 */
void MonteCarlo::delta(const PnlMat *past, double t, PnlVect *delta, PnlVect *conf_delta){

    double sum;
    double sum2;
    double timestep = opt_->T_/(opt_->nbTimeSteps_);
    double ic;
    double coefficient;
    double prix;
    double payoff_increment;
    double payoff_decrement;

    PnlMat *path = pnl_mat_create(opt_->nbTimeSteps_ + 1, mod_->size_);
    PnlMat *increment_path = pnl_mat_create(opt_->nbTimeSteps_ + 1, mod_->size_);
    PnlMat *decrement_path = pnl_mat_create(opt_->nbTimeSteps_ + 1, mod_->size_);

    pnl_mat_set_subblock(path, past, 0, 0);
    int a = 0;
    double step_1 = t * opt_->nbTimeSteps_ / opt_->T_ ;
    double nbSteps = floor(t * opt_->nbTimeSteps_ / opt_->T_);
    if (nbSteps == step_1) {
      nbSteps += 1 ;
    }
    else{
      nbSteps += 2 ;
      a = 1;
    }

    for (int d = 0; d < mod_->size_; d++) {
        sum = 0;
        sum2 = 0;
        prix = pnl_mat_get(past, past->m - 1, d);
        coefficient = exp(-mod_->r_ * (opt_->T_ - t))/(2 * fdStep_ * prix);
        for (size_t i = 0; i < nbSamples_; i++) {
            mod_->asset(path, t, opt_->T_, opt_->nbTimeSteps_, rng_, past, nbSteps, a);
            mod_->shiftAsset(increment_path, path, d, fdStep_, t, timestep);
            mod_->shiftAsset(decrement_path, path, d, -fdStep_, t, timestep);
            payoff_increment = opt_->payoff(increment_path);
            payoff_decrement = opt_->payoff(decrement_path);
            sum += payoff_increment - payoff_decrement;
            sum2 += pow(payoff_increment - payoff_decrement, 2);
        }
        pnl_vect_set(delta, d, coefficient * sum / nbSamples_);
        double standard_dev = coefficient * sqrt(sum2 / nbSamples_ - pow(sum/nbSamples_, 2));
        pnl_vect_set(conf_delta, d, standard_dev / sqrt(nbSamples_));
    }

    pnl_mat_free(&path);
    pnl_mat_free(&increment_path);
    pnl_mat_free(&decrement_path);
}

/**
 * Calcule le prix et le delta des options à tout instant donné
 *
 * @param[out] listPrice contient le prix des options à differents instants
 * @param[in] matDelta contient le delta des options à differents instants
 * @param[in] marketPrice contient la disposition des trajectoires de marché
 * @param[in] H : nombre de rebalancements
 */
void MonteCarlo::PriceDelta(PnlVect *listPrice, PnlMat *matDelta, PnlMat *marketPrice, int H){
        double delta_h = (opt_->T_)/H;
        double delta_t = (opt_->T_)/opt_->nbTimeSteps_;
        int k = H/opt_->nbTimeSteps_;
        int indice_t = 0;
        double ic = 0;
        double prix = 0;
        PnlMat *past = pnl_mat_create(1, opt_->size_);
        PnlVect *value_h = pnl_vect_create(opt_->size_);
        PnlVect *listDelta = pnl_vect_create(opt_->size_);
        PnlVect *confDelta = pnl_vect_create(opt_->size_);
        for (int i = 0; i < H+1; i++) {
          pnl_mat_get_row(value_h, marketPrice, i);
          pnl_mat_set_row(past, value_h, indice_t);
          price(past, i*delta_h,  prix , ic);

          delta(past, i*delta_h, listDelta, confDelta);
          if (i%k == 0) {
            pnl_mat_add_row(past, past->m, value_h);
            indice_t += 1;
          }
          pnl_vect_set(listPrice, i, prix);
          pnl_mat_set_row(matDelta, listDelta, i);
        }
        pnl_vect_free(&value_h);
        pnl_mat_free(&past);
        pnl_vect_free(&confDelta);
        pnl_vect_free(&listDelta);
}

/**
 * Construction du portefeuille de couverture
 * Calcul de l'évolution de la part investie au taux sans risque
 *
 * @param[out] listHedge contient la valeur du portefeuille de couverture à differents instants
 * @param[in] marketPrice contient la disposition des trajectoires de marché
 */
void MonteCarlo::listHedge(PnlVect *listHedge,PnlVect *lastDelta, double& lastPrice, PnlMat *marketPrice){
        double H = marketPrice->m - 1;
        int size = marketPrice->n;
        PnlVect *price = pnl_vect_create(H+1);
        PnlMat *deltas = pnl_mat_create(H+1,size);
        PriceDelta(price, deltas, marketPrice, H);
        pnl_mat_get_row(lastDelta, deltas, H);
        lastPrice = pnl_vect_get(price, H);
        double price_h = pnl_vect_get(price, 0);
        PnlVect *deltaSize_prec = pnl_vect_create(size);
        PnlVect *deltaSize = pnl_vect_create(size);
        PnlVect *valueSize = pnl_vect_create(size);
        pnl_mat_get_row(deltaSize_prec, deltas, 0);
        pnl_mat_get_row(valueSize,marketPrice, 0);
        double hedgeValue ;
        hedgeValue = price_h - pnl_vect_scalar_prod(deltaSize_prec, valueSize);
        pnl_vect_set(listHedge, 0, hedgeValue);

        for (int i = 1; i < H+1; i++) {
          pnl_mat_get_row(deltaSize, deltas, i);
          pnl_mat_get_row(valueSize,marketPrice, i);
          pnl_vect_minus(deltaSize_prec);
          pnl_vect_plus_vect(deltaSize_prec, deltaSize);
          hedgeValue = hedgeValue * exp(mod_->r_ * opt_->T_/H) - pnl_vect_scalar_prod(deltaSize_prec, valueSize);
          pnl_vect_clone(deltaSize_prec, deltaSize);
          pnl_vect_set(listHedge, i, hedgeValue);
        }

        pnl_vect_free(&price);
        pnl_mat_free(&deltas);
        pnl_vect_free(&deltaSize_prec);
        pnl_vect_free(&deltaSize);
        pnl_vect_free(&valueSize);
}

/**
* Profit and Loss
* Calcul de l'erreur de couverture
* @param[out]  pnl : erreur de couverture
* @param[in] marketPrice : disposition des trajectoires de marché
* @param[in] H : nombre de rebalancements
*/
void MonteCarlo::pnl(double& pnl, PnlMat *marketPrice, int H){
      double lastPrice = 0;
      PnlVect* lastDelta = pnl_vect_create(opt_->size_);
      PnlVect* hedges = pnl_vect_create(H+1);
      listHedge(hedges, lastDelta, lastPrice, marketPrice);
      PnlVect* rowPrice = pnl_vect_create(opt_->size_);
      pnl_mat_get_row(rowPrice, marketPrice, H);

      pnl = pnl_vect_get(hedges, H) + pnl_vect_scalar_prod(lastDelta, rowPrice) - lastPrice;

      pnl_vect_free(&lastDelta);
      pnl_vect_free(&hedges);
      pnl_vect_free(&rowPrice);
}
