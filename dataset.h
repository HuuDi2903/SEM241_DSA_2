/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt
 * to change this license Click
 * nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this
 * template
 */

/*
 * File:   dataset.h
 * Author: ltsach
 *
 * Created on September 2, 2024, 3:59 PM
 */

#ifndef DATASET_H
#define DATASET_H
#include "tensor/xtensor_lib.h"
using namespace std;

// Todo CODE YOUR

template <typename DType, typename LType>
class DataLabel {
 private:
  xt::xarray<DType> data;
  xt::xarray<LType> label;

 public:
  DataLabel(xt::xarray<DType> data, xt::xarray<LType> label)
      : data(data), label(label) {}
  xt::xarray<DType> getData() const { return data; }
  xt::xarray<LType> getLabel() const { return label; }
};

template <typename DType, typename LType>
class Batch {
 private:
  xt::xarray<DType> data;
  xt::xarray<LType> label;

 public:
  Batch(xt::xarray<DType> data, xt::xarray<LType> label)
      : data(data), label(label) {}
  virtual ~Batch() {}
  xt::xarray<DType>& getData() { return data; }
  xt::xarray<LType>& getLabel() { return label; }
};

template <typename DType, typename LType>
class Dataset {
 private:
 public:
  Dataset() {};
  virtual ~Dataset() {};

  virtual int len() = 0;  // This method returns the size of the dataset; that is, how many samples (observations) are in the dataset.
  virtual DataLabel<DType, LType> getitem(int index) = 0;  // This method returns an object of type DataLabel.
  virtual xt::svector<unsigned long> get_data_shape() = 0;  // change to size_t
  virtual xt::svector<unsigned long> get_label_shape() = 0;  // change to size_t
};

//////////////////////////////////////////////////////////////////////
template <typename DType, typename LType>
class TensorDataset : public Dataset<DType, LType> {
 private:
  xt::xarray<DType> data;
  xt::xarray<LType> label;
  xt::svector<unsigned long> data_shape, label_shape;  // change to size_t

 public:
  TensorDataset(xt::xarray<DType> data, xt::xarray<LType> label) {
    // */ When not initialized or initialized with a scalar value (a number), the tensor is just a scalar.
    //       Has a shape: an empty tuple
    //       Has a dimension: 0
    //       Has a size: 1, because it contains one number.
    //       The scalar can be accessed via: x or x[0], where x is the variable name. */

    this->data = data;
    this->label = label;
    this->data_shape = data.shape();
    this->label_shape = label.shape();
  }

  int len() {
    // TODO implement
    return this->data_shape[0];
  }

  DataLabel<DType, LType> getitem(int index) {
    // // TODO implement

    // // Data is available but labels are not.
    // // If there is only one label, every data has that label.
    // // Data and labels are both available.
    // // The size of dimension 0 in both the data and label tensors must be the same.
    // getitem(): returns a pair of data and label; the label may be missing (dimension: 0)

      // Index out of bounds
      if (index <0 || index >= this->len()) {
          throw out_of_range("Index is out of range!");
      }

      xt::xarray<DType> sampleData = xt::view(this->data, index);
      xt::xarray<LType> sampleLabel;

      if (this->label.dimension() == 0) {
          // Case 1: The label is missing
          sampleLabel = this->label;
      } else {
          // Case 2: The label is available
          sampleLabel = xt::view(this->label, index);
      }

      return DataLabel<DType, LType>(sampleData, sampleLabel);
  }

  xt::svector<unsigned long> get_data_shape() { return this->data_shape; }  // change to size_t

  xt::svector<unsigned long> get_label_shape() { return this->label_shape; }  // change to size_t
};

#endif /* DATASET_H */
