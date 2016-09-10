//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  General small utility functions.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///   
///  \author Pat Scott  
///          (patscott@physics.mcgill.ca)
///  \date 2013 Apr, July, Aug, Dec
///  \date 2014 Mar
///  \date 2015 Apr
///
///  \author Ben Farmer
///          (benjamin.farmer@monash.edu.au)
///  \date 2013 May, June, July
///
///  *********************************************


#ifndef __util_functions_hpp__
#define __util_functions_hpp__

#include <vector>
#include <chrono> 
#include <cmath>

#include "gambit/Utils/util_types.hpp"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

extern "C"
{
  #include "mkpath/mkpath.h"  
}

# if GAMBIT_CONFIG_FLAG_use_std_regex
  #include <regex>
  #define GAMBIT_CONFIG_FLAG_use_regex 1
  namespace Gambit { using std::regex; using std::regex_replace; }
#elif GAMBIT_CONFIG_FLAG_use_boost_regex
  #include <boost/regex.hpp>
  #define GAMBIT_CONFIG_FLAG_use_regex 1
  namespace Gambit { using boost::regex; using boost::regex_replace; }
#else
  #include <boost/algorithm/string/replace.hpp>
  #define GAMBIT_CONFIG_FLAG_use_regex 0
#endif

namespace Gambit
{

  /// Redirection function to turn an lvalue into an rvalue, so that it
  /// is correctly passed by value when doing perfect forwarding with
  /// functor typecasting.
  template <typename T>
  T byVal(T t) { return t; }

  template <typename T> 
  int sgn(T val) { return (T(0) < val) - (val < T(0)); }

  /// Make sure there are no nasty surprises from regular C abs()
  using std::abs;

  namespace Utils
  {

    /// Split a string into a vector of strings, using a delimiter,
    /// and removing any whitespace around the delimiter.
    std::vector<str> delimiterSplit(str s, str delim);

    /// Strips namespace from the start of a string, or after "const".
    str strip_leading_namespace(str s, str ns);

    /// Strip all whitespace except that following "const", 
    /// in which case the whitespace is replaced by a single space.
    void strip_whitespace_except_after_const(str&);

    /// Strips leading and/or trailing parentheses from a string.
    void strip_parentheses(str&);

    /// Created a str of a specified length.
    str str_fixed_len(str, int);
    
    /// Copy a str to a character array, stripping the null termination character.
    void strcpy2f(char*, int, str);

    /// Get pointers to beginning and end of array. 
    // Useful for initialising vectors with arrays, e.g.
    //   int vv[] = { 12,43 };
    //   std::vector<int> v(beginA(vv), endA(vv));
    // Though 'begin' is unnecessary, can just do
    //   std::vector<int> v(vv, endA(vv));
    template <typename T, size_t N>
    T* beginA(T(&arr)[N]) { return &arr[0]; }
    template <typename T, size_t N>
    T* endA(T(&arr)[N]) { return &arr[0]+N; }

    /// Test if two sets are disjoint (works on any sorted std container I think)
    // From http://stackoverflow.com/questions/1964150/c-test-if-2-sets-are-disjoint
    template<class Set1, class Set2> 
    bool is_disjoint(const Set1 &set1, const Set2 &set2)
    {
      if(set1.empty() || set2.empty()) return true;
  
      typename Set1::const_iterator 
          it1 = set1.begin(), 
          it1End = set1.end();
      typename Set2::const_iterator 
          it2 = set2.begin(), 
          it2End = set2.end();
  
      if(*it1 > *set2.rbegin() || *it2 > *set1.rbegin()) return true;
  
      while(it1 != it1End && it2 != it2End)
      {
          if(*it1 == *it2) return false;
          if(*it1 < *it2) { it1++; }
          else { it2++; }
      }
  
      return true;
    }

     /// Ensure that a path exists (and then return the path, for chaining purposes)
    const str& ensure_path_exists(const str&);

    /// Return a vector of strings listing the contents of a directory (POSIX)
    std::vector<str> ls_dir(const str& dir);

    /// Get directory name from full path+filename (POSIX)
    str dir_name(const str& path);

    /// Get file name from full path+filename (POSIX)
    str base_name(const str& path);

    /// Delete all files in a directory (does not act recursively)
    int remove_all_files_in(const str& dirname, bool error_if_absent = true);

    typedef std::chrono::time_point<std::chrono::system_clock> time_point;

    /// Get clock time
    time_point get_clock_now();

    /// Get date and time
    str return_time_and_date(const time_point& in);

    /// Check if two strings are a "close" match
    /// Used for "did you mean?" type checking during command line argument processing
    bool are_similar(const str& s1, const str& s2);   

    /// Sub-check for are_similar.
    /// true if s1 can be obtained by deleting one character from s2
    bool check1(const str& s1, const str& s2);   

    /// Sub-check for are_similar.
    /// true if s1 can be obtained from s2 by changing no more than X characters (X=2 for now)    
    bool check2(const str& s1, const str& s2);

    /// returns square of double - saves tedious repetition
    double sqr(double a);
    
    /// Local GAMBIT definition of isnan.  Could be redefined at a later point, depending on compiler support. 
    using std::isnan;

    /// Local GAMBIT definition of isinf.  Could be redefined at a later point, depending on compiler support. 
    using std::isinf;

  }

}
        
#endif //defined __util_functions_hpp__
