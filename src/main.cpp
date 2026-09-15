#include <cstdlib>
#include "SharedPtr.h"

int main() {
   //copy 
   SharedPtr<int> sPtr3(new int(5));
   SharedPtr<int> sPtr4 = sPtr3;
   SharedPtr<int> sPtr5 = SharedPtr<int>(sPtr4);
   assert(*sPtr3 == 5);
   assert(*sPtr4 == 5);
   assert(*sPtr5 == 5);
   assert(sPtr3 == sPtr5); // == operator

   //move
   SharedPtr<int> sPtr1(new int(5));
   SharedPtr<int> sPtr2(std::move(sPtr1));
   assert(sPtr1.get()  == nullptr);
   assert(*sPtr2 == 5);
   SharedPtr<int> sPtr6 = std::move(sPtr2);
   assert(sPtr2.get() == nullptr);
   assert(*sPtr6 == 5);
   assert(bool(sPtr6) != false); // != and bool operators

   //reset
   
   return EXIT_SUCCESS;
}
