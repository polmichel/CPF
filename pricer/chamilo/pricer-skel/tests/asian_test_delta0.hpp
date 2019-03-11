#ifndef CHAMILO_TEST_ASIAN_DELTA0
#define CHAMILO_TEST_ASIAN_DELTA0
#include <gtest/gtest.h>
#include "jlparser/parser.hpp"
#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"

/**
* Programme de test pour le delta en 0 d'une option asiatique
* Comparaison avec les résultats exacts donnés dans les fichiers .dat
*/

TEST(MonteCarlo, AsianDelta0){
    double fdStep = 0.1;
    double T, r, strike, correlation;
    PnlVect *spot, *sigma, *divid, *payoff_coef;
    string type;
    int size, timestep;
    size_t n_samples;

    const char *infile = "../data/asian.dat";
    Param *P = new Parser(infile);

    P->extract("option type", type);
    P->extract("maturity", T);
    P->extract("option size", size);
    P->extract("spot", spot, size);
    P->extract("correlation", correlation);
    P->extract("volatility", sigma, size);
    P->extract("interest rate", r);
    if (P->extract("dividend rate", divid, size, true) == false)
    {
        divid = pnl_vect_create_from_zero(size);
    }
    P->extract("strike", strike);
    P->extract("timestep number", timestep);
    P->extract("sample number", n_samples);
    P->extract("payoff coefficients", payoff_coef, size);
    PnlVect *trend = pnl_vect_create_from_zero(size);

    BlackScholesModel *bsmodel = new BlackScholesModel(size, r, correlation, sigma, spot, trend);
    Option *aOption = new AsianOption(T, timestep, size, payoff_coef, strike);
    PnlRng *rng= pnl_rng_create(PNL_RNG_MERSENNE);

    pnl_rng_init(rng, PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, time(NULL));
    MonteCarlo *mCarlo = new MonteCarlo(bsmodel, aOption, rng, fdStep, n_samples);

    PnlMat *past = pnl_mat_create_from_scalar(1, 2, 100);
    PnlVect *delta = pnl_vect_create(2);
    PnlVect *conf_delta = pnl_vect_create(2);


    mCarlo->delta(past, 0, delta, conf_delta);

    printf("Delta actif %u: %f\n", 1, pnl_vect_get(delta, 0));
    printf("Standard Deviation actif %u: %f\n", 1, pnl_vect_get(conf_delta, 0));
    //ASSERT_LE(0.281640 - ic, GET(delta, 0)) << "Error, delta of first option not in confidence interval, too low";
    //ASSERT_GE(0.281640 + ic, GET(delta, 0)) << "Error, delta of first option not in confidence interval, too high";
    printf("Delta actif %u: %f\n", 2, pnl_vect_get(delta, 1));
    printf("Standard Deviation actif %u: %f\n", 2, pnl_vect_get(conf_delta, 1));
    //ASSERT_LE(0.281951 - ic, GET(delta, 1)) << "Error, delta of second option not in confidence interval, too low";
    //ASSERT_GE(0.281951 + ic, GET(delta, 1)) << "Error, delta of second option not in confidence interval, too high";

    pnl_vect_free(&spot);
    pnl_vect_free(&sigma);
    pnl_vect_free(&divid);
    pnl_rng_free(&rng);
    pnl_mat_free(&past);
    pnl_vect_free(&delta);
    pnl_vect_free(&conf_delta);

    delete P;
    delete bsmodel;
    delete aOption;
    delete mCarlo;
}


#endif //CHAMILO_TEST_ASIAN_PRICE0
