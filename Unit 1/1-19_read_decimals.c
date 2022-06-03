#include <stdio.h>
int main(void) {
    double height;
    printf("How tall are you (in meters)? ");
    scanf("%lf", &height);
    printf("I am %.2lf meters tall.", height);
    return 0;
}