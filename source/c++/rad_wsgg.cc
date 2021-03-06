/**
 * @file rad_wsgg.cc
 * Source file for class rad_wsgg
 */

#include <vector>
#include <iostream>
#include <cmath>
#include "rad_wsgg.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////////

const double rad_wsgg::cCoefs[100]={ 7.412956e-001, -5.244441e-001,  5.822860e-001, -2.096994e-001,  2.420312e-002,
                                    -9.412652e-001,  2.799577e-001, -7.672319e-001,  3.204027e-001, -3.910174e-002,
                                     8.531866e-001,  8.230754e-002,  5.289430e-001, -2.468463e-001,  3.109396e-002,
                                    -3.342806e-001,  1.474987e-001, -4.160689e-001,  1.697627e-001, -2.040660e-002,
                                     4.314362e-002, -6.886217e-002,  1.109773e-001, -4.208608e-002,  4.918817e-003,
                                     1.552073e-001, -4.862117e-001,  3.668088e-001, -1.055508e-001,  1.058568e-002,
                                     6.755648e-001,  1.409271e+000, -1.383449e+000,  4.575210e-001, -5.019760e-002,
                                    -1.125394e+000, -5.913199e-001,  9.085441e-001, -3.334201e-001,  3.842361e-002,
                                     6.040543e-001, -5.533854e-002, -1.733014e-001,  7.916083e-002, -9.893357e-003,
                                    -1.105453e-001,  4.646634e-002, -1.612982e-003, -3.539835e-003,  6.121277e-004,
                                     2.550242e-001,  3.805403e-001, -4.249709e-001,  1.429446e-001, -1.574075e-002,
                                    -6.065428e-001,  3.494024e-001,  1.853509e-001, -1.013694e-001,  1.302441e-002,
                                     8.123855e-001, -1.102009e+000,  4.046178e-001, -8.118223e-002,  6.298101e-003,
                                    -4.532290e-001,  6.784475e-001, -3.432603e-001,  8.830883e-002, -8.415221e-003,
                                     8.693093e-002, -1.306996e-001,  7.414464e-002, -2.029294e-002,  2.010969e-003,
                                    -3.451994e-002,  2.656726e-001, -1.225365e-001,  3.001508e-002, -2.820525e-003,
                                     4.112046e-001, -5.728350e-001,  2.924490e-001, -7.980766e-002,  7.996603e-003,
                                    -5.055995e-001,  4.579559e-001, -2.616436e-001,  7.648413e-002, -7.908356e-003,
                                     2.317509e-001, -1.656759e-001,  1.052608e-001, -3.219347e-002,  3.386965e-003,
                                    -3.754908e-002,  2.295193e-002, -1.600472e-002,  5.046318e-003, -5.364326e-004};  

const double rad_wsgg::dCoefs[20]={ 3.404288e-002,  6.523048e-002, -4.636852e-002,  1.386835e-002, -1.444993e-003,
                                    3.509457e-001,  7.465138e-001, -5.293090e-001,  1.594423e-001, -1.663261e-002,
                                    4.570740e+000,  2.168067e+000, -1.498901e+000,  4.917165e-001, -5.429990e-002,
                                    1.098169e+002, -5.092359e+001,  2.343236e+001, -5.163892e+000,  4.393889e-001};

///////////////////////////////////////////////////////////////////////////////
/** THIS IS THE CLASS INTERFACE FUNCTION
 *  Given the gas state, set the k and a vectors.
 *  These can then be accessed by the user.
 *  return through arg list the local gray gas coefficients (kabs) and the local weights (awts).
 *  @param T      \input gas temperature (K)
 *  @param P      \input pressure (Pa)
 *  @param xH2O   \input mole fraction H2O
 *  @param xCO2   \input mole fraction CO2
 *  @param xCO    \input mole fraction CO     HERE FOR THE INTERFACE, NOT USED (... pass in 0.0)
 *  @param xCH4   \input mole fraction CH4    HERE FOR THE INTERFACE, NOT USED (... pass in 0.0)
 *  @param fvsoot \input soot volume fraction = rho*Ysoot/rhosoot
 *  @param kabs   \output absorption coefficients (1/m) for nGG+1 (nGG gray gases + clear gas)
 *  @param awts   \output weights (unitless; sums to 1) for nGG+1 (nGG gray gases + clear gas)
 */

void rad_wsgg::get_k_a(const double   T_dmb,
                       const double   P,
                       const double   xH2O,
                       const double   xCO2,
                       const double   xCO_not_used,
                       const double   xCH4_not_used,
                       const double   fvsoot,
                       vector<double> &kabs,
                       vector<double> &awts) {

    //------------------------

    double Mr = xH2O/(xCO2+1E-6);
    if(Mr<0.01) Mr = 0.01;
    if(Mr>4)    Mr = 4;
    cout << endl << "Mr = " << Mr << endl; //doldb

    double T = T_dmb;
    if(T<500)  T = 500;
    if(T>2400) T = 2400;

    double Tr = T/1200;

    const int ni  = 4;
    const int nj  = 5;
    const int nk  = 5;

    kabs = vector<double>(nGGa,0.0);
    awts = vector<double>(nGGa,0.0);
    vector<vector<double> > b(ni, vector<double>(nj,0.0));

    int njnk = nj*nk;
    int injnk;
    int off;
    for(int i=0; i<ni; i++) {
        injnk = i*njnk;
        for(int j=0; j<nj; j++){
            off = injnk + j*nk;
            b[i][j] = cCoefs[off+0] + Mr*(cCoefs[off+1] + Mr*(cCoefs[off+2] + Mr*(cCoefs[off+3] + Mr*(cCoefs[off+4])))); // notationally, cCoefs terms are like cCoefs[i][j][k] where k is +0,+1,etc.
        }
    }
    
    awts[0] = 1.0;
    for(int i=1; i<nGGa; i++){
        awts[i] = b[i-1][0] + Tr*(b[i-1][1] + Tr*(b[i-1][2] + Tr*(b[i-1][3] + Tr*(b[i-1][4]))));
        awts[0] -= awts[i];
    }

    for(int i=1; i<nGGa; i++){
        off = (i-1)*nk;
        kabs[i] = dCoefs[off+0] + Mr*(dCoefs[off+1] + Mr*(dCoefs[off+2] + Mr*(dCoefs[off+3] + Mr*(dCoefs[off+4]))));
        kabs[i] *= (P/101325)*(xH2O+xCO2);
    }

    return;
}
