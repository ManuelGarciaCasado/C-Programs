#include <stdio.h>
int main(void) {
    int i;
    // i++ is short for i = i+1
    for (i = 3; i>0; i = i-1) {
        printf("i has the value %d.\n", i);
    }
    return 0;
}
