/*
Write a C-program that reads an input character (using scanf) and displays the following pyramid pattern using the character read: 

Examples
Input
#
Output
++++#++++
+++###+++
++#####++
+#######+
#########
Input
o
Output
++++o++++
+++ooo+++
++ooooo++
+ooooooo+
ooooooooo
*/
#include <stdio.h>

int main(void) {

    char l;

    scanf("%c", &l);

    printf("++++%c++++\n", l);
    printf("+++%c%c%c+++\n", l, l, l);
    printf("++%c%c%c%c%c++\n", l, l, l, l, l);
    printf("+%c%c%c%c%c%c%c+\n", l, l, l, l, l, l, l);
    printf("%c%c%c%c%c%c%c%c%c\n", l, l, l, l, l, l, l, l, l);

    return(0);
}