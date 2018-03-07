#include "vec3.h"
#include <math.h>

float Vec3f::getL() const{
    float ans = this->x*this->x + this->y*this->y + this->z*this->z;
    ans = sqrt(ans);
    return ans;
}

float Vec3f::dot(const Vec3f& v) const{
    float ans = this->x * v.x;
    ans += this->y * v.y;
    ans += this->z * v.z;
    return ans;
}

Vec3f Vec3f::add(const Vec3f& v) const{
    Vec3f ans(this->x + v.x, this->y + v.y, this->z + v.z);
    return ans;
}

Vec3f operator+(const Vec3f& a, const Vec3f& b){
    return a.add(b);
}

Vec3f Vec3f::scale(float n) const{
    Vec3f ans(this->x * n, this->z * n, this->z * n);
    return ans;
}

Vec3f operator*(const Vec3f& v, float n){
    return v.scale(n);
}

Vec3f operator*(float n, const Vec3f& v){
    return v.scale(n);
}

