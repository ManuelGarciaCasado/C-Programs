/*Please arrange the following program segments in the correct order so that it prints "I love love love programming!"

Then use comments to label the pieces of the program (preprocessor directive, variable declaration, executable statement, return statement, main function).*/
#include <stdio.h> //Preprocessor Directive

int main(void) { //Main Function

    int i; //Variable Declaration

    printf("I "); //Executable Statement
    for(i = 0; i < 3 ; i++) {
        printf("love "); //Executable Statement
    }
    printf("programming!"); //Executable Statement

    return(0); //Return Statement
}
