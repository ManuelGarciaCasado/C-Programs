#include <stdio.h>
int main(void) {
    int diceValue = 0;
    int nbThrows = 0;
    scanf("%d", &diceValue);
    while(diceValue != 6){
        scanf("%d", &diceValue);
        nbThrows = nbThrows + 1;
    }
    printf("We needed %d throws to get the value 6\n", nbThrows+1);
    return 0;
}