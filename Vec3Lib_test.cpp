#include "aclib/vec3.h"
#include "aclib/aclib.h"
#include "aclib/point3.h"

#include <stdio.h>

void printVec3ln(Vec3f v){
    printf("(%f,%f,%f)\n", v.x,v.y,v.z);
}
void printFloatln (float f){
    printf("%f\n", f);
}

int main(){
    Vec3f v1 = Vec3f(1,2,3);
    printVec3ln(v1);
    Vec3f v2 = Vec3f();
    printVec3ln(v2);
    Vec3f v3 = Vec3f(3,2,-1);

    printVec3ln(v3+v1);

    printVec3ln(v3-v1);
    printVec3ln(v1-v3);
    printVec3ln(-v3);

    printVec3ln(v3*5);
    printVec3ln(5*v3);
    printFloatln(v1*v3);

    printVec3ln(v1/v3);
    printVec3ln(v3/v1);

    printFloatln(v3.getL());
    printVec3ln(v3.getUnit());
    printVec3ln(v3.getUnitFast());

    Point3f p1(1.0,2.0,3.0);
    Point3f p2(3.0,2.0,1.0);

    printVec3ln(Vec3f(p1));
    printVec3ln(Vec3f(p1,p2));
    

    return 0;
}



// int main(){
//     X x1;
//     printf("%d\n", x1.testfunc());
//     return 1;
// }

