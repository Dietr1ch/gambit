/*
 * A dummy C++ library for testing GAMBIT backend setup 
 * 
 * \author Anders Kvellestad
 * \date 2013-03-26
 * 
 */

#include <iostream>
#include <string>

//
// Some global variables:
//
int someInt;
double someDouble;
bool isInitialized = false;
std::string prefix = "libfirst: ";

//
// Some functions:
//

// 'initialization'
void initialize(int a)
{
  std::cout << std::endl;
  std::cout << prefix << "This is function 'initialize'." << std::endl;
  someInt = a;
  isInitialized = true;
  std::cout << prefix << "Initialization done. Variable 'someInt' set to: " << someInt << std::endl;
} // end initialize


// 'calculation'
void someFunction()
{
  std::cout << std::endl;
  std::cout << prefix << "This is function 'someFunction'." << std::endl;

  if (isInitialized)
  {
    std::cout << prefix << "Will now perform a calculation..." << std::endl;
    someDouble = 3.1415*someInt;
    std::cout << prefix << "Result stored in variable 'someDouble' is: " << someDouble << std::endl;
  }
  else
  {
    std::cout << prefix << "Not initialized. Cannot perform calculation." << std::endl;
  }
} // end someFunction


// return 'result'
double returnResult()
{
  return someDouble;
}


