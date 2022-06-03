#include <stdio.h>
int main(void) {
    //! word1 = showArray(word1, cursors=[i], width=0.5)
    //! word2 = showArray(word2, cursors=[i], width=0.5)
    char word1[5];
    char word2[8];
    scanf("%s %s", word1, word2);
    word1[3] = '\0';
    word2[2] = '\0';
    printf("%s %s\n", word1, word2);
    return 0;
}