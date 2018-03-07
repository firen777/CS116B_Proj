#pragma once
/**Author: Un Hou (Albert) Chan
 * 
*/

/**3D Vector, float version
*/
class Vec3f
{
    public:
        const float x;
        const float y;
        const float z;
    public:
        /**Constructor
        */
        Vec3f(float _x, float _y, float _z):x(_x), y(_y), z(_z){}
        /**Get Length of Vector
         * @return length
        */
        float getL() const;
        /**vector dot product, this dot v
         * @param v
         * @return a float
        */
        float dot(const Vec3f& v) const;
        /**vector cross product, this X v
         * @param v 
         * @return a new Vec3f
        */ 
        Vec3f cross(const Vec3f& v) const;
        /**vector addition, this + v
         * @param v
         * @return a new Vec3f
        */
        Vec3f add(const Vec3f& v) const;
        friend Vec3f operator+(const Vec3f& a, const Vec3f& b);
        /**vector scaling, nV
         * @return a new Vec3f
        */
        Vec3f scale(float n) const;
        friend Vec3f operator*(const Vec3f& v, float n);
        friend Vec3f operator*(float n, const Vec3f& v);
        

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