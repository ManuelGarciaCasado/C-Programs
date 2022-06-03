/*You are rolling a red, a yellow and a green standard dice (whose sides each have the numbers 1-6 on them) and combine them to a 3-digit number by using the red value as the ones place, the yellow value as the tens place and the green value as the hundreds place. For example, if you rolled a 3 with red, a 5 with yellow and 2 with green then the combined 3-digit number would be 253.

You repeat this process and your job is to continuously find the average of all of those 3-digit numbers.

More precisely, you need to first read an integer that tells you how many 3-digit numbers you'll be averaging. Next, you need to read the red value, then the yellow value, then the green value of the first throw and print out the corresponding 3-digit number as well as the current average with one decimal place. Then you repeat this process for the given number of rounds. Here is a sample run:

Input
3
3 5 2
4 2 1
1 6 4

Output
1. you rolled: 253, current average: 253.0
2. you rolled: 124, current average: 188.5
3. you rolled: 461, current average: 279.3
*/
#include <stdio.h>

int main(void) {
    int N, i, dice, score, sum;
    double ave;
    sum = 0;
    int weight1 = 1;
    int weight2 = 10;
    int weight3 = 100;
    scanf("%d", &N);
    for (i=0; i<N; i++) {
        score = 0;
        scanf("%d", &dice);
        score += dice*weight1;
        scanf("%d", &dice);
        score += dice*weight2;
        scanf("%d", &dice);
        score += dice*weight3;
        sum += score;
        ave = ((double) sum)/(i+1);
        printf("%d. you rolled: %d, current average: %.1lf\n", i+1, score, ave);
    }
    return 0;
}