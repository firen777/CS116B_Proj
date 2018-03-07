#include "vec3lib/vec3.h"
//gcc vec3lib/*.cpp
#include <stdio.h>

int main(){
    Vec3f v1(1,2,3);
    Vec3f v2(1.0,1.1,1.2);
    printf("%f,\n%f\n",v1.x,v2.z);
    printf("%f\n", v2.getL());
    Vec3f v3 = v1 + v2;
    printf("%f, %f\n", v3.x, v3.getL());
    return 0;
}

// int main(){
//     X x1;
//     printf("%d\n", x1.testfunc());
//     return 1;
// }

