#ifndef BLOCK_ARRAY_H
#define BLOCK_ARRAY_H

#include <stdlib.h>
#include <stdio.h>
#include <mutex>

class BlockArray {
public:
  BlockArray();
  void * get(long index);
  void   set(long index, void * data);
  size_t positiveArrayLength;
  size_t negativeArrayLength;
private:
  void** positiveArray;
  void** negativeArray;
  std::mutex mtx;
};

#endif
