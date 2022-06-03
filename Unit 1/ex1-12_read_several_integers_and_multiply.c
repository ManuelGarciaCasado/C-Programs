/*n this activity, you want to improve your existing multiplication program (that prints the 8 times table). Your program should read an integer from the user (not you) and print the multiplication table for the number that they enter. To test if your program works, we will try running your code with several different inputs so make sure you don't hard code the solution.

Example 1
Input:
8
Output:
0x8 = 0
1x8 = 8
...
10x8 = 80
Example 2
Input:
5
Output:
0x5 = 0
1x5 = 5
...
10x5 = 50*/
#include <stdio.h>
int main() {
    int i, num;
    scanf("%d", &num);
    for (i = 0; i <= 10; i++){
        printf("%dx%d = %d\n", i, num, i*num);
	}
    return(0);
}
