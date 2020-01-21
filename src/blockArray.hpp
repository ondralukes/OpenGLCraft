#ifndef BLOCK_ARRAY_H
#define BLOCK_ARRAY_H

#include <stdlib.h>
#include <stdio.h>

class BlockArray {
public:
  BlockArray();
  void * get(long index);
  void   set(long index, void * data);
private:
  void** positiveArray;
  void** negativeArray;
  size_t positiveArrayLength;
  size_t negativeArrayLength;
};

#endif
