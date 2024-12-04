/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt
 * to change this license Click
 * nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this
 * template
 */

/*
 * File:   ReLU.cpp
 * Author: ltsach
 *
 * Created on August 25, 2024, 2:44 PM
 */

#include "layer/ReLU.h"

#include "ann/functions.h"
#include "sformat/fmt_lib.h"

ReLU::ReLU(string name) {
  if (trim(name).size() != 0)
    m_sName = name;
  else
    m_sName = "ReLU_" + to_string(++m_unLayer_idx);
}

ReLU::ReLU(const ReLU& orig) { m_sName = "ReLU_" + to_string(++m_unLayer_idx); }

ReLU::~ReLU() {}

/*  M =X≥0 
    Y =M⊙X 
    DX =M⊙DY
*/

/*
xt::xarray<bool> b = { false, true, true, false };
xt::xarray<int> a1 = { 1,   2,  3,  4 };
xt::xarray<int> a2 = { 11, 12, 13, 14 };

xt::xarray<int> res = xt::where(b, a1, a2);
// => res = { 11, 2, 3, 14 }
*/

xt::xarray<double> ReLU::forward(xt::xarray<double> X) {
  // Todo CODE YOUR
  this->m_aMask = xt::where(X >= 0, 1.0, 0.0);

  return xt::where(m_aMask, X, 0.0);
}


xt::xarray<double> ReLU::backward(xt::xarray<double> DY) {
  // Todo CODE YOUR
  xt::xarray<double> DX = this->m_aMask * DY;

  return DX;
}

string ReLU::get_desc() {
  string desc = fmt::format("{:<10s}, {:<15s}:", "ReLU", this->getname());
  return desc;
}