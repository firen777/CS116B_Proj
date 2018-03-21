/**Author: Un Hou (Albert) Chan
 * Custom made 3D vector class
 * Dependancy: "aclib.h", "point3.h"
*/
#pragma once
#include "aclib.h"
#include "point3.h"

/**3D Vector, float version
 * Constructor:
 * Vec3f(x,y,z);
 * Vec3f(); Default constructor with vector(0,0,0)
 * Vec3f(p); Point3f to Vec3f
 * Vec3f(p1, p2); 2 Point3f to Vec3f 
 * 
 * Methods:
 * float getL(); get Length of vector
 * Vec3f getUnit(); get the unit vector with the same direction, AKA normalized vector
 * static iVec(), jVec(), kVec(); factory method for i,j,k vector.
 * 
 * Overloaded Operators:
 * Vec3f operator+(Vec3f, Vec3f); Vector addition
 * Vec3f operator-(Vec3f, Vec3f); Vector subtraction, or adding opposite vector
 * Vec3f operator-(Vec3f); opposite vector
 * Vec3f operator*(Vec3f, float) or (float, Vec3f); Vector scaling
 * float operator*(Vec3f, Vec3f); dot product
 * Vec3f operator/(Vec3f, Vec3f); cross product
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
        Vec3f(float _x, float _y, float _z):
            x(_x), y(_y), z(_z){}
        Vec3f():
            x(0.0f), y(0.0f), z(0.0f){}
        Vec3f(const Point3f& p):
            x(p.x), y(p.y), z(p.z){}
        Vec3f(const Point3f& p1, const Point3f& p2): 
            x(p2.x-p1.x), y(p2.y-p1.y), z(p2.z-p1.z){}

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

        /*Produce i,j,k base Vectors.
        */
        /**@deprecated static allocation only. Please use constructor.
        */
        static Vec3f iVec();
        /**@deprecated static allocation only. Please use constructor.
        */
        static Vec3f jVec();
        /**@deprecated static allocation only. Please use constructor.
        */
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