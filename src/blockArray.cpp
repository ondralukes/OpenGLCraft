#include "blockArray.hpp"

BlockArray::BlockArray(){
  positiveArrayLength = 0;
  negativeArrayLength = 0;
  positiveArray = NULL;
  negativeArray=NULL;
}

void *
BlockArray::get(long index){
  if(index >= 0){
    if(index >= positiveArrayLength){
      return NULL;
    } else{
      return positiveArray[(int)index];
    }
  } else {
    if(-index-1 >= negativeArrayLength){
      return NULL;
    } else {
      return negativeArray[(-index-1)];
    }
  }
}

void
BlockArray::set(long index, void * data){
  if(index >= 0){
    if(index >= positiveArrayLength){
      size_t newLength = (index+1)*15/10;
      positiveArray = (void**)realloc(positiveArray,sizeof(void *)*newLength);
      for(size_t i = positiveArrayLength;i<newLength;i++){
        positiveArray[i] = NULL;
      }
      positiveArrayLength = newLength;

    }
    positiveArray[index] = data;
  } else {
    if(-index-1 >= negativeArrayLength){
      size_t newLength = (-index)*15/10;
      negativeArray = (void**)realloc(negativeArray,sizeof(void *)*newLength);
      for(size_t i = negativeArrayLength;i<newLength;i++){
        negativeArray[i] = NULL;
      }
      negativeArrayLength = newLength;
    }
    negativeArray[(-index-1)] = data;
  }
}
