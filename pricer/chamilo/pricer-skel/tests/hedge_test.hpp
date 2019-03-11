//
// Created by lecalvmy on 9/20/18.
//

#ifndef MC_PRICER_TEST_PRICE_T_H
#define MC_PRICER_TEST_PRICE_T_H

#include <gtest/gtest.h>
#include "jlparser/parser.hpp"
#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"


/**
* Programme de test pour la couverture d'un Call
*/
TEST(MonteCarlo, pnlCall){

  const char *infile = "../market-soutenance/market_call.dat";
  PnlMat *callPath = pnl_mat_create_from_file(infile);

  double fdStep = 0.1; // valeur quelconque car non utilisee pour ce test

  int size = 1;
  double r = 0.05;
  double rho = 0;
  PnlVect *sigma = pnl_vect_create_from_scalar(size,0.200000);
  PnlVect *spot = pnl_vect_create_from_scalar(size,100.000000);
  PnlVect *trend = pnl_vect_create_from_scalar(size, 0.05);

  double T = 0.5;
  int nbTimeSteps = 1;
  int H = 150;
  double strike = 100;

  PnlRng *rng= pnl_rng_create(PNL_RNG_MERSENNE);
  pnl_rng_init(rng, PNL_RNG_MERSENNE);
  pnl_rng_sseed(rng, time(NULL));

  size_t n_samples = 100000;

  PnlVect *weights = pnl_vect_create_from_scalar(1, 1.0);

  BlackScholesModel *bsmodel = new BlackScholesModel(size, r, rho, sigma, spot, trend);
  Option *call = new AsianOption(T, nbTimeSteps, size, weights, strike);
  PnlMat *path = pnl_mat_create(H+1, size);
  bsmodel->simul_market(path, T, rng);

  MonteCarlo *mCarlo = new MonteCarlo(bsmodel, call, rng, fdStep, n_samples);
  double pnl = 0;
  mCarlo->pnl(pnl, callPath, H);
  printf("P&L: %f\n", pnl);

  pnl_mat_free(&callPath);
  delete call;
  delete bsmodel;
  delete mCarlo;

}

TEST(MonteCarlo, pnlAsian){
  const char *infile = "../market-soutenance/market_asian.dat";
  PnlMat *Path = pnl_mat_create_from_file(infile);

  double fdStep = 0.1; // valeur quelconque car non utilisee pour ce test

  int size = 1;
  double r = 0.03;
  double rho = 0.3;
  PnlVect *sigma = pnl_vect_create_from_scalar(size,0.200000);
  PnlVect *spot = pnl_vect_create_from_scalar(size,100.000000);
  PnlVect *trend = pnl_vect_create_from_scalar(size, 0.1);

  PnlVect *weights = pnl_vect_create_from_scalar(1, 1.0);

  double T = 2;
  int nbTimeSteps = 26;
  int H = 104;
  double strike = 100;

  PnlRng *rng= pnl_rng_create(PNL_RNG_MERSENNE);
  pnl_rng_init(rng, PNL_RNG_MERSENNE);
  pnl_rng_sseed(rng, time(NULL));

  size_t n_samples = 100000;

  BlackScholesModel *bsmodel = new BlackScholesModel(size, r, rho, sigma, spot, trend);
  Option *option = new AsianOption(T, nbTimeSteps, size, weights, strike);

  MonteCarlo *mCarlo = new MonteCarlo(bsmodel, option, rng, fdStep, n_samples);
  double pnl = 0;
  mCarlo->pnl(pnl, Path, H);
  printf("P&L: %f\n", pnl);

  pnl_mat_free(&Path);
  delete option;
  delete bsmodel;
  delete mCarlo;
}

TEST(MonteCarlo, pnlBasket){
  const char *infile = "../market-soutenance/market_basket.dat";
  PnlMat *Path = pnl_mat_create_from_file(infile);

  double fdStep = 0.1; // valeur quelconque car non utilisee pour ce test

  int size = 10;
  double r = 0.04879;
  double rho = 0.3;
  PnlVect *sigma = pnl_vect_create_from_scalar(size,0.200000);
  PnlVect *spot = pnl_vect_create_from_scalar(size,100.000000);
  PnlVect *trend = pnl_vect_create_from_scalar(size, 0.1);

  double T = 2;
  int nbTimeSteps = 1;
  int H = 150;
  double strike = 100;

  PnlRng *rng= pnl_rng_create(PNL_RNG_MERSENNE);
  pnl_rng_init(rng, PNL_RNG_MERSENNE);
  pnl_rng_sseed(rng, time(NULL));

  size_t n_samples = 100;
  PnlVect *weights = pnl_vect_create_from_scalar(10, 0.1);


  BlackScholesModel *bsmodel = new BlackScholesModel(size, r, rho, sigma, spot, trend);
  Option *option = new BasketOption(T, nbTimeSteps, size, weights, strike);

  MonteCarlo *mCarlo = new MonteCarlo(bsmodel, option, rng, fdStep, n_samples);
  double pnl = 0;
  mCarlo->pnl(pnl, Path, H);
  printf("P&L: %f\n", pnl);

  pnl_mat_free(&Path);
  delete option;
  delete bsmodel;
  delete mCarlo;
}

#endif //MC_PRICER_TEST_PRICE_T_H
