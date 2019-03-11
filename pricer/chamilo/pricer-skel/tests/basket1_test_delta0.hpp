#ifndef CHAMILO_TEST_BASKET1_DELTA0
#define CHAMILO_TEST_BASKET1_DELTA0
#include <gtest/gtest.h>
#include "jlparser/parser.hpp"
#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"

/**
* Programme de test pour le delta d'une option basket
*/

TEST(MonteCarlo, Basket1Delta0){
    double fdStep = 0.1;
    double T, r, strike, correlation;
    PnlVect *spot, *sigma, *divid, *payoff_coef;
    string type;
    int size, timestep;
    size_t n_samples;

    const char *infile = "../data/basket_1.dat";
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
    Option *bOption = new BasketOption(T, timestep, size, payoff_coef, strike);
    PnlRng *rng= pnl_rng_create(PNL_RNG_MERSENNE);
    //
    pnl_rng_init(rng, PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, time(NULL));
    MonteCarlo *mCarlo = new MonteCarlo(bsmodel, bOption, rng, fdStep, n_samples);

    PnlMat *past = pnl_mat_create_from_scalar(1, size, 100);

    const char *infile2 = "../data/basket_1_delta.dat";
    const PnlVect *delta_attendu = pnl_vect_create_from_file(infile2);

    PnlVect *delta = pnl_vect_create(size);
    PnlVect *conf_delta = pnl_vect_create(size);

    mCarlo->delta(past, 0, delta, conf_delta);
    int good_sum = 0;
    double ic = 0;
    for (int i =0; i < size; i ++){
        printf("Delta actif %u: %f\n", i+1, pnl_vect_get(delta, i));
        printf("Standard Deviation actif %u: %f\n", i+1, pnl_vect_get(conf_delta, i));
        /*ic = pnl_vect_get(conf_delta, i);
        ASSERT_LE(pnl_vect_get(delta_attendu, i) - ic, GET(delta, i)) << "Error, delta of first option not in confidence interval, too low";
        ASSERT_GE(pnl_vect_get(delta_attendu, i) + ic, GET(delta, i)) << "Error, delta of first option not in confidence interval, too high";
        if (pnl_vect_get(delta_attendu, i) - ic <= GET(delta, i) && GET(delta, i) <= pnl_vect_get(delta_attendu, i) + ic) {
            good_sum +=1;
        }*/
    }

    pnl_vect_free(&spot);
    pnl_vect_free(&sigma);
    pnl_vect_free(&divid);
    pnl_rng_free(&rng);
    pnl_mat_free(&past);
    pnl_vect_free(&delta);
    pnl_vect_free(&conf_delta);

    delete P;
    delete bsmodel;
    delete bOption;
    delete mCarlo;
}


#endif //CHAMILO_TEST_ASIAN_PRICE0
