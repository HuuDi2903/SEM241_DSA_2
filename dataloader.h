/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt
 * to change this license Click
 * nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this
 * template
 */

/*
 * File:   dataloader.h
 * Author: ltsach
 *
 * Created on September 2, 2024, 4:01 PM
 */

#ifndef DATALOADER_H
#define DATALOADER_H
#include "loader/dataset.h"
#include "tensor/xtensor_lib.h"

using namespace std;

// Todo CODE YOUR

template <typename DType, typename LType>
class DataLoader {
 public:
  class Iterator;
 private:
  Dataset<DType, LType>* ptr_dataset;
  int batch_size;
  bool shuffle;
  bool drop_last;
  int m_seed;
  // TODO : add more member variables to support the iteration
  int num_batches;
  xt::xarray<int> indexArray;  
  xt::xarray<LType> my_label;
  xt::xarray<DType> my_data;  

 public:
  // Task 4 helper
  int get_total_batch() { 
    return this->num_batches; 
  }

  DataLoader(Dataset<DType, LType>* ptr_dataset, int batch_size,
             bool shuffle = true, bool drop_last = false, int seed = -1) : ptr_dataset(ptr_dataset),
             batch_size(batch_size), shuffle(shuffle), drop_last(drop_last) {
    // TODO implement

    this->m_seed = seed;

    // auto xt::arange(T stop),
    // Generates numbers evenly spaced within given half-open interval [0, stop).
    this->indexArray = xt::arange<int>(ptr_dataset->len());  

    // Call the function xt::random::seed(m_seed) IMMEDIATELY BEFORE calling the function xt::random::shuffle, 
    // with the conditions: shuffle=true and seed >= 0.
    if (shuffle) {
      if (seed >= 0) {
        xt::random::seed(seed);
        xt::random::shuffle(indexArray);
      } else {
        xt::random::shuffle(indexArray);
      }
    }

    // if (shuffle) {
    //   xt::random::default_engine_type engine(0);
    //   xt::random::shuffle(indexArray, engine);
    // }

    
    // Calculate number of batches
    num_batches = ptr_dataset->len() / batch_size;
  }

  virtual ~DataLoader() {
    // TODO implement
    // delete this->ptr_dataset;
  }

  Iterator begin() {
    // TODO implement
    return Iterator(this, 0);
  }

  Iterator end() {
    // TODO implement
    // int num_samples = ptr_dataset->len();
    
    // // If drop_last is true, we only consider full batches.
    // int total_batches = drop_last ? total_samples / batch_size : (total_samples + batch_size - 1) / batch_size;
    
    return Iterator(this, num_batches);
  }

  // TODO implement forech
  class Iterator {
  private:
    // TODO implement
    DataLoader<DType, LType> *pLoader;
    int current_batch;  // index of the current batch
  
  public:
    // TODO implement contructor
    Iterator(DataLoader<DType, LType>* pLoader, int current_batch) : pLoader(pLoader),
    current_batch(current_batch) {}   

    Iterator& operator=(const Iterator& iterator) {
      // TODO implement
      this->pLoader = iterator.pLoader;
      this->current_batch = iterator.current_batch;
      return *this;
    }

    Iterator& operator++() {
      // TODO implement
      this->current_batch++;
      return *this;
    }

    Iterator operator++(int) {
      // TODO implement
      Iterator iterator = *this;
      ++(*this);
      return iterator;
    }

    bool operator!=(const Iterator& other) const {
      // TODO implement
      return this->current_batch != other.current_batch;
    }
    
    // 1. Data without labels: In this case, there is data, but no labels are provided.
    // 2. Data with labels: In this case, it is always assumed that the number of labels is equal to the number of data points. 
    // That is, the size of dimension 0 of the data and labels is the same.
    Batch<DType, LType> operator*() const {
        int num_samples = pLoader->ptr_dataset->len();
        int start_index = current_batch * pLoader->batch_size;
        int end_index = start_index + pLoader->batch_size;

        // If drop_last is false and this batch is the last batch, we include the remaining samples in the last batch.
        if (!pLoader->drop_last && current_batch == pLoader->num_batches - 1) {
            end_index += num_samples % pLoader->batch_size;
        }

        // Prepare containers for data and labels
        DataLabel<DType, LType> new_batch = pLoader->ptr_dataset->getitem(pLoader->indexArray(start_index));

        xt::xarray<DType> batch_data = new_batch.getData();
        batch_data = xt::expand_dims(batch_data, 0);  // Turn this into a vector

        xt::xarray<LType> batch_label;


        // Case 1: Data without labels
        if (pLoader->ptr_dataset->get_label_shape().size() == 0) {
          // Label is a empty array
          batch_label = xt::xarray<LType>();

          for (int i = start_index + 1; i < end_index; ++i) { 
            // Get the i-th sample
            DataLabel<DType, LType> temp_batch = pLoader->ptr_dataset->getitem(pLoader->indexArray(i));

            // Get the data of the i-th sample
            xt::xarray<DType> temp_batch_data = temp_batch.getData();
            temp_batch_data = xt::expand_dims(temp_batch_data, 0);

            // Concatenate the data of the i-th sample to the batch data
            xt::xarray<DType> added_data = xt::concatenate(xt::xtuple(batch_data, temp_batch_data), 0);

            // Update the batch data
            batch_data = added_data;
          }
        } 
        else {  // Case 2: Data with labels
          // Get the label of the first sample
          batch_label = new_batch.getLabel();
          batch_label = xt::expand_dims(batch_label, 0);

          for (int i = start_index + 1; i < end_index; ++i) {
            // Get the i-th sample
            DataLabel<DType, LType> temp_batch = pLoader->ptr_dataset->getitem(pLoader->indexArray(i));

            // Get the data and label of the i-th sample
            xt::xarray<DType> temp_batch_data = temp_batch.getData();
            xt::xarray<LType> temp_batch_label = temp_batch.getLabel();
            temp_batch_data = xt::expand_dims(temp_batch_data, 0);
            temp_batch_label = xt::expand_dims(temp_batch_label, 0);

            // Concatenate the data and label of the i-th sample to the batch data and label
            xt::xarray<DType> added_data = xt::concatenate(xt::xtuple(batch_data, temp_batch_data), 0);
            xt::xarray<LType> added_label = xt::concatenate(xt::xtuple(batch_label, temp_batch_label), 0);

            // Update the batch data and label
            batch_data = added_data;
            batch_label = added_label;
          }    
        }

        return Batch<DType, LType>(batch_data, batch_label);
    }
  };
};

#endif /* DATALOADER_H */
