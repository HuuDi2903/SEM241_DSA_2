/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt
 * to change this license Click
 * nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this
 * template
 */

/*
 * File:   Softmax.cpp
 * Author: ltsach
 *
 * Created on August 25, 2024, 2:46 PM
 */

#include "layer/Softmax.h"

#include <filesystem>  //require C++17

#include "ann/functions.h"
#include "sformat/fmt_lib.h"
namespace fs = std::filesystem;

Softmax::Softmax(int axis, string name) : m_nAxis(axis) {
  if (trim(name).size() != 0)
    m_sName = name;
  else
    m_sName = "Softmax_" + to_string(++m_unLayer_idx);
}

Softmax::Softmax(const Softmax& orig) {}

Softmax::~Softmax() {}
/*
xt::xarray<double> softmax(xt::xarray<double> X, int axis) {
  xt::svector<unsigned long> shape = X.shape();
  axis = positive_index(axis, shape.size());
  shape[axis] = 1;

  xt::xarray<double> Xmax = xt::amax(X, axis);
  X = xt::exp(X - Xmax.reshape(shape));
  xt::xarray<double> SX = xt::sum(X, -1);
  SX = SX.reshape(shape);
  X = X / SX;

  return X;
}
*/
xt::xarray<double> Softmax::forward(xt::xarray<double> X) {
  // Todo CODE YOUR
  xt::xarray<double> Y = softmax(X, this->m_nAxis);
  if (this->m_trainable) {
    this->m_aCached_Y = Y;
  }

  return Y;
}

xt::xarray<double> Softmax::backward(xt::xarray<double> DY) {
  // Todo CODE YOUR
  // ∆z =(DIAG(y) − y ⊗ y^T) × ∆y
  xt::xarray<double> Y = this->m_aCached_Y;
  // double batch_size = Y.shape()[0];
  xt::xarray<double> DZ = xt::zeros_like(DY);

  if (DY.shape().size() == 1) {
    DZ = (xt::diag(Y) - xt::linalg::outer(Y, Y)) * DY;
  } else {
      xt::xarray<double> diag_Y = diag_stack(Y);
      xt::xarray<double> outer_Y = outer_stack(Y, Y);
      DZ = matmul_on_stack(diag_Y - outer_Y, DY); //0.72
  }

  return DZ;
}


string Softmax::get_desc() {
  string desc = fmt::format("{:<10s}, {:<15s}: {:4d}", "Softmax",
                            this->getname(), m_nAxis);
  return desc;
}
