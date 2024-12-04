/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt
 * to change this license Click
 * nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this
 * template
 */

/*
 * File:   CrossEntropy.cpp
 * Author: ltsach
 *
 * Created on August 25, 2024, 2:47 PM
 */

#include "loss/CrossEntropy.h"

#include "ann/functions.h"

CrossEntropy::CrossEntropy(LossReduction reduction) : ILossLayer(reduction) {}

CrossEntropy::CrossEntropy(const CrossEntropy& orig) : ILossLayer(orig) {}

CrossEntropy::~CrossEntropy() {}

// double cross_entropy(xt::xarray<double> Ypred, xt::xarray<double> Ygt,
//                      bool mean_reduced) {
//   int nsamples = Ypred.shape()[0];
//   xt::xarray<double> logYpred = xt::log(Ypred + 1e-7);
//   xt::xarray<double> R = -Ygt * logYpred;
//   R = xt::sum(R, -1);

//   xt::xarray<double> sum = xt::sum(R);
//   if (mean_reduced)
//     return (sum / nsamples)[0];
//   else
//     return sum[0];
// }

// /*
//  */
// double cross_entropy(xt::xarray<double> Ypred, xt::xarray<unsigned long> ygt,
//                      bool mean_reduced) {
//   int nclasses = Ypred.shape()[1];
//   xt::xarray<double> Ytarget = onehot_enc(ygt, nclasses);

//   return cross_entropy(Ypred, Ytarget, mean_reduced);
// }

// xt::xarray<double> onehot_enc(xt::xarray<unsigned long> x, int nclasses) {
//   int nsamples = x.shape()[0];
//   xt::xarray<double> Y = xt::zeros<double>({nsamples, nclasses});
//   for (int r = 0; r < nsamples; r++) {
//     int c = x[r];
//   xt:
//     view(Y, r, c) = 1.0;
//   }
//   return Y;
// }

/*
xt::xarray<double> one_hot_encoded = {
  {1.0, 0.0, 0.0},  // Sample 1 belongs to class 0
  {0.0, 0.0, 1.0},  // Sample 2 belongs to class 2
  {0.0, 1.0, 0.0}   // Sample 3 belongs to class 1
};
*/ 

  /*
  t = {1, 0}
  --> t.shape() = {2}
  --> t.shape().size() = 1
  --> t.shape()[0] = 2
  */
 
double CrossEntropy::forward(xt::xarray<double> X, xt::xarray<double> t) {
  // Todo CODE YOUR
  this->m_aCached_Ypred = X;
  this->m_aYtarget = t;

  double loss;
  if (this->m_eReduction == REDUCE_MEAN) {
    loss = cross_entropy(X, t, true);
  } else {
    loss = cross_entropy(X, t, false);
  }

  return loss;
}

// ∆y = -(1/Nnorm) x (t/(y + epsilon))
xt::xarray<double> CrossEntropy::backward() {
  // Todo CODE YOUR
  xt::xarray<double> y = this->m_aCached_Ypred;
  xt::xarray<double> t = this->m_aYtarget;
  double N_norm = y.shape()[0];
  xt::xarray<double> dy;

  if (this->m_eReduction == REDUCE_MEAN) {
    dy = -(1.0 / N_norm) * (t / (y + 1e-7));
  } else {
    dy = -t / (y + 1e-7);
  }

  return dy;
}
