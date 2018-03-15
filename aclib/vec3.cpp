/**Author: Un Hou (Albert) Chan
 * Dependancy: "aclib.h"
*/

#include "vec3.h"
#include "aclib.h"

#include <math.h>

#include <stdio.h>

float Vec3f::getL() const{
    float ans = this->x*this->x + this->y*this->y + this->z*this->z;
    ans = sqrt(ans);
    return ans;
}

Vec3f Vec3f::add(const Vec3f& v) const{
    Vec3f ans(this->x + v.x, this->y + v.y, this->z + v.z);
    return ans;
}

Vec3f operator+(const Vec3f& a, const Vec3f& b){
    return a.add(b);
}

Vec3f Vec3f::getNeg() const{
    return *this * -1.0f;
}

Vec3f Vec3f::operator-() const{
    return *this * -1.0f;
}

Vec3f operator-(const Vec3f& a, const Vec3f& b){
    return a.add(b.getNeg());
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

float Vec3f::dot(const Vec3f& v) const{
    float ans = this->x * v.x;
    ans += this->y * v.y;
    ans += this->z * v.z;
    return ans;
}

float operator*(const Vec3f& v1, const Vec3f& v2){
    return v1.dot(v2);
}

Vec3f Vec3f::cross(const Vec3f& v) const{
    Vec3f icomp = Vec3f::iVec();
    Vec3f jcomp = Vec3f::jVec();
    Vec3f kcomp = Vec3f::kVec();

    icomp = icomp * (this->y*v.z - this->z*v.y);
    jcomp = jcomp * (this->x*v.z - this->z*v.x);
    kcomp = kcomp * (this->x*v.y - this->y*v.x);

    Vec3f ans = icomp - jcomp + kcomp;

    return ans;
}

Vec3f operator/(const Vec3f& v1, const Vec3f& v2){
    return v1.cross(v2);
}

Vec3f Vec3f::getUnit() const{
    if (this->x==0.0f && this->y==0.0f && this->z==0.0f){
        return *this;
    }
    float length = this->x*this->x + this->y*this->y + this->z*this->z;
    length = sqrt(length);

    Vec3f ans = this->scale(1.0f/length);
    return ans;
}

Vec3f Vec3f::getUnitFast() const{
    if (this->x==0.0f && this->y==0.0f && this->z==0.0f){
        return *this;
    }

    float length = this->x*this->x + this->y*this->y + this->z*this->z;
    
    Vec3f ans = this->scale(aclib::fast_invsqrt(length)); //weird negative bug when using union version. wtf?
    
    // printf("DEBUG:%f\n",(length));
    // float n = aclib::Q_rsqrt(length);
    // printf("DEBUG:%f\n",n);

    // Vec3f ans = this->scale(n);

    // printf("DEBUG:%f\n",(length));
    // printf("DEBUG:%f\n",aclib::Q_rsqrt(length));
    // printf("DEBUG:%f\n",aclib::Q_rsqrt(length));
    
    return ans;
}

Vec3f Vec3f::iVec(){
    return Vec3f(1.0, 0.0, 0.0);
}

Vec3f Vec3f::jVec(){
    return Vec3f(0.0, 1.0, 0.0);
}

Vec3f Vec3f::kVec(){
    return Vec3f(0.0, 0.0, 1.0);
}

