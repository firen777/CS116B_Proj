#include "vec3.h"
#include <math.h>


float Vec3f::getL(){
    float ans = this->x*this->x + this->y*this->y + this->z*this->z;
    ans = sqrt(ans);
    return ans;
}

float Vec3f::dot(const Vec3f& v){
    float ans = this->x * v.x;
    ans += this->y * v.y;
    ans += this->z * v.z;
    return ans;
}

Vec3f Vec3f::add(const Vec3f& v){
    Vec3f ans(this->x + v.x, this->y + v.y, this->z + v.z);
    return ans;
}

Vec3f operator+(const Vec3f& a, const Vec3f& b){
    Vec3f ans(a.x+b.x, a.y+b.y, a.z+b.z);
    return ans;
}