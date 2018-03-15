/**Author: Un Hou (Albert) Chan
 * Custom made 3D vector class
 * Dependancy: "aclib.h"
*/
#pragma once
#include "aclib.h"

/**3D Vector, float version
*/
class Vec3f
{
    public:
        float x;
        float y;
        float z;
    public:
        /*Constructors
        */
        Vec3f(float _x, float _y, float _z):x(_x), y(_y), z(_z){}
        Vec3f():x(0.0f), y(0.0f), z(0.0f){}

        /**Get Length of Vector
         * @return length
        */
        float getL() const;

        /**vector addition, this + v
         * @param v
         * @return a new Vec3f
        */
        Vec3f add(const Vec3f& v) const;
        friend Vec3f operator+(const Vec3f& a, const Vec3f& b);

        /**Get the negative Vector -(this)
         * @return -(this)
        */
        Vec3f getNeg() const;
        Vec3f operator-() const;
        /**vector subtraction
        */
        friend Vec3f operator-(const Vec3f& a, const Vec3f& b);

        /**vector scaling, nV
         * @return a new Vec3f
        */
        Vec3f scale(float n) const;
        friend Vec3f operator*(const Vec3f& v, float n);
        friend Vec3f operator*(float n, const Vec3f& v);

        /**vector dot product, this dot v
         * @param v
         * @return a float
        */
        float dot(const Vec3f& v) const;
        /**vector dot product, v1 dot v2
        */
        friend float operator*(const Vec3f& v1, const Vec3f& v2);
        
        /**vector cross product, this X v
         * @param v 
         * @return a new Vec3f
        */ 
        Vec3f cross(const Vec3f& v) const;
        /**vector cross product, v1 X v2
        */
        friend Vec3f operator/(const Vec3f& v1, const Vec3f& v2);
        
        /**Get the unit vector w/ the same direction of this vector
         * @return a new Vec3f unit vector. Return *this if this vector is [0.0f, 0.0f, 0.0f]
        */
        Vec3f getUnit() const;

        /**EXPERIMENTAL! Unit vector using Fast Inverse Squareroot Algorithm
         * Reference: https://en.wikipedia.org/wiki/Fast_inverse_square_root
         * @return a new Vec3f unit vector. Return *this if this vector is [0.0f, 0.0f, 0.0f]
        */
        Vec3f getUnitFast() const;

        /*Factory Methods
          Produce i,j,k base Vectors.
        */
        static Vec3f iVec();
        static Vec3f jVec();
        static Vec3f kVec();

        
};

/**
 * 3D Vector, integer point version
*/
/*
class Vec3i
{
    private:
    public:
        int x;
        int y;
        int z;
    public:
        Vec3i(int _x, int _y, int _z):x(_x), y(_y), z(_z){}
        int getL();
        int dot(const Vec3i& v);
        Vec3i cross(const Vec3i& v);
};
*/

// class testClass {
// public:
//     int x;
// public:
//     testClass():x(0){}
//     testClass(int n):x(n){}

//     int getX() {return this->x;}

//     friend testClass operator+(testClass T1, testClass T2){
//         return testClass(T1.x + T2.x);
//     }
// };