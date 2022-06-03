#include <stdio.h>
int main(void) {
    int diceValue;
    int notSix;
    scanf("%d", &diceValue);
    notSix = diceValue != 6;
    while (notSix) {
        scanf("%d", &diceValue);
        notSix = diceValue != 6;
    }
    return 0;
}
