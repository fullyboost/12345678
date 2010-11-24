// inverse_gamma_example.cpp

// Copyright Paul A. Bristow 2010.

// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// Example 1 of using inverse gamma functions.

#include <boost/math/special_functions/gamma.hpp>

using boost::math::gamma_p_inv; // Compute x given a
//using boost::math::gamma_q_inv;
//using boost::math::gamma_p_inva; // Compute a given x
//using boost::math::gamma_q_inva;

#include <iostream>
   using std::cout;    using std::endl;
#include <iomanip>
   using std::setprecision;
#include <cmath>
   using std::sqrt;
#include <limits>

int main()
{
  cout << "Example 1 using Inverse Gamma function. " << endl;

  cout.precision(std::numeric_limits<double>::max_digits10);
  
  double x = 1.;
  double a = 10;

  double r = boost::math::gamma_q_inv(a ,x);

  cout << " x = " << x << ", = gamma_q_inv(a,x)" << r << endl; // 
  
  return 0;
}  // int main()

/*

Output is:

*/


