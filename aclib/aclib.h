/**Author: Un Hou (Albert) Chan
 * collection of useful helper functions
*/
#pragma once


namespace aclib{

    /**Fast inverse square root
     * reference: https://en.wikipedia.org/wiki/Fast_inverse_square_root
     * good for C standard. may be undefined behavior in C++ but should be fine.
     * @return 1/sqrt(number)
    */
    float Q_rsqrt( float number );
}