#include "aclib/vec3.h"
#include "aclib/aclib.h"

#include <stdio.h>

void printVec3ln(Vec3f v){
    printf("%f,%f,%f,\n", v.x,v.y,v.z);
}



int main(){
    Vec3f v1(1,-2,3);
    Vec3f v2(1,1,1);
    // printVec3ln(v1);
    // printf("v1 L: %f\n", v1.getL());
    // printVec3ln(v1+v2);
    // printVec3ln(v1*3);
    // printVec3ln(3*v1);
    // printf("v1 dot v2: %f\n", v1.dot(v2));

    // printf("fast_invsqrt: (0.111111, 0.25, 1): (%f,%f,%f)\n", aclib::fast_invsqrt(0.1111f),aclib::fast_invsqrt(0.25f),aclib::fast_invsqrt(1.0f));

    Vec3f vi = Vec3f::iVec();
    Vec3f vj = Vec3f::jVec();
    Vec3f vk = Vec3f::kVec();

    printVec3ln(vi);
    printVec3ln(vj);
    printVec3ln(vk);

    printVec3ln(vi.cross(vj));
    printVec3ln(vk.cross(vi));
    printVec3ln(vj.cross(vk));

    Vec3f v4_1 = v1.getUnit();
    Vec3f v4_2 = v1.getUnitFast();

    printVec3ln(v4_1);
    printVec3ln(v4_2);

    printVec3ln(v4_1.cross(v4_1));

    printVec3ln(Vec3f(1,2,3).cross(Vec3f(4,5,6)));


    return 0;
}



// int main(){
//     X x1;
//     printf("%d\n", x1.testfunc());
//     return 1;
// }

