/**Author: Un Hou (Albert) Chan
 * Custom made 3D point class
 * Dependancy: "aclib.h"
*/

#pragma once
#include "aclib.h"

/**3D Point, float version
 * 
*/
class Point3f
{
    public:
        float x;
        float y;
        float z;

    public:
        /*Constructors
        */
        Point3f(float _x, float _y, float _z):x(_x), y(_y), z(_z){}
        Point3f():x(0.0f), y(0.0f), z(0.0f){}

};