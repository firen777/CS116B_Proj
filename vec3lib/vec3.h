/**Author: Un Hou (Albert) Chan
 * 
*/

/**
 * 3D Vector, floating point version
*/
class Vec3f
{
    private:
    public:
        float x;
        float y;
        float z;
    public:
        Vec3f(float _x, float _y, float _z):x(_x), y(_y), z(_z){}
        float getL();
        float dot(const Vec3f& v);
        Vec3f cross(const Vec3f& v);
};

/**
 * 3D Vector, integer point version
*/
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