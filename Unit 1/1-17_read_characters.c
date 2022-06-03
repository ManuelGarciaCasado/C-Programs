#include <stdio.h>
int main(void) {
    char letter, letter2;
    printf("Please enter two letters separated by a comma: ");
    scanf("%c,%c", &letter, &letter2);
    printf("I read the letters %c and %c.\n", letter, letter2);
    return 0;
}