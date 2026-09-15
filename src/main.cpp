#include <cstdlib>
#include <cassert>
#include <utility>
#include "SharedPtr.h"

int main() {
   //move
   SharedPtr<int> sPtr1(new int(5));
   SharedPtr<int> sPtr2(std::move(sPtr1));
   assert(sPtr1.get() == nullptr);
   assert(*(sPtr2.get()) == 5);
   
   //copy 
   SharedPtr<int> sPtr3(new int(5));
   SharedPtr<int> sPtr4 = sPtr1;
   assert(*(sPtr3.get()) == 5);
   assert(*(sPtr4.get()) == 5);
   return EXIT_SUCCESS;
}
