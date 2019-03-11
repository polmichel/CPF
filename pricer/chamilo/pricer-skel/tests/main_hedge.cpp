
#include <gtest/gtest.h>
#include "../src/BlackScholesModel.cpp"
#include "../src/MonteCarlo.cpp"
#include "../src/AsianOption.cpp"
#include "../src/BasketOption.cpp"
#include "../src/CallOption.cpp"
#include "../src/PerformanceOption.cpp"
#include "hedge_test.hpp"


int main(int argc,char *argv[]) {
    ::testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}
