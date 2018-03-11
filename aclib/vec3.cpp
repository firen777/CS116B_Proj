#include "vec3.h"
#include "aclib.h"
#include <math.h>

#include <stdio.h>

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

Vec3f Vec3f::cross(const Vec3f& v) const{
    //TODO: Implement cross product
    return Vec3f(1,2,3);
}

Vec3f Vec3f::add(const Vec3f& v) const{
    Vec3f ans(this->x + v.x, this->y + v.y, this->z + v.z);
    return ans;
}

Vec3f operator+(const Vec3f& a, const Vec3f& b){
    return a.add(b);
}

Vec3f Vec3f::scale(float n) const{
    Vec3f ans(this->x * n, this->y * n, this->z * n);
    return ans;
}

Vec3f operator*(const Vec3f& v, float n){
    return v.scale(n);
}

Vec3f operator*(float n, const Vec3f& v){
    return v.scale(n);
}

Vec3f Vec3f::getUnit() const{
    float length = this->x*this->x + this->y*this->y + this->z*this->z;
    length = sqrt(length);

    Vec3f ans = this->scale(1.0f/length);
    return ans;
}

Vec3f Vec3f::getUnitFast() const{
    float length = this->x*this->x + this->y*this->y + this->z*this->z;
    
    Vec3f ans = this->scale(aclib::Q_rsqrt(length)); //weird negative bug when using union version. wtf?
    
    // printf("DEBUG:%f\n",(length));
    // float n = aclib::Q_rsqrt(length);
    // printf("DEBUG:%f\n",n);

    // Vec3f ans = this->scale(n);

    // printf("DEBUG:%f\n",(length));
    // printf("DEBUG:%f\n",aclib::Q_rsqrt(length));
    // printf("DEBUG:%f\n",aclib::Q_rsqrt(length));
    
    return ans;
}

