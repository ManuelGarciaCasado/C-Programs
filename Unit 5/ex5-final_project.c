//In this project you will work with the linked list of digits we have created in the lessons during this course. As before you are provided with some code that you should not modify:
//
//A structure definition for the storage of each digit's information.
//
//A main() function to test your code. 
//
//The functions createDigit(), append(), printNumber(), freeNumber() and readNumber() which we have written in the lectures.
//
//Your task is to write a new function reverseNumber() which receives as input a pointer that holds the address of the start of a linked list of digits and that returns a pointer that holds the address of the start of a new linked list of digits, namely the original list but in reverse order.
//
//Hint: You should sit down with a piece of paper and a pen first to design the steps necessary to solve this problem. What additional functions might be useful? Do not start coding right away. For example, it may help to write function which finds the length of a linked list of digits. Another useful function might be one which finds the nth digit in a linked list of digits. Think this through first before starting to code.
//
//Examples:
//
// 
//
//Input
//
//35267
//Output
//
//The reverse of 35267
//is 76253
//Input
//
//5
//Output
//
//The reverse of 5
//is 5
#include <stdio.h>
#include <stdlib.h>

struct digit {
    int num;
    struct digit *next;
};

struct digit *reverseNumber(struct digit *start); 
int getLength(struct digit *start);
int getNth(struct digit *start, int n);
struct digit *createDigit(int dig);
struct digit * append(struct digit * end, struct digit * newDigptr);
void printNumber(struct digit *start);
void freeNumber(struct digit *start);
struct digit *readNumber(void);

int main(void) {
    struct digit *start, *backwards;
    start = readNumber();
    backwards = reverseNumber(start);
    printf("The reverse of ");
    printNumber(start);
    printf("is ");
    printNumber(backwards);
        
    freeNumber(start);
    freeNumber(backwards);
    return 0;
}

struct digit *createDigit(int dig) {
    struct digit *ptr;
    ptr = (struct digit *) malloc(sizeof(struct digit));
    ptr->num = dig;
    ptr->next = NULL;
    return ptr;
}

struct digit * append(struct digit * end, struct digit * newDigptr) {
    end->next = newDigptr;
    return(end->next);
}

void printNumber(struct digit *start) {
    struct digit * ptr = start;
    while (ptr!=NULL) {
        printf("%d", ptr->num);
        ptr = ptr->next;
    }
    printf("\n");
}

void freeNumber(struct digit *start) {
    struct digit * ptr = start;
    struct digit * tmp;
    while (ptr!=NULL) {
        tmp = ptr->next;
        free(ptr);
        ptr = tmp;
    }
}

struct digit *readNumber(void) {
    char c;
    int d;
    struct digit *start, *end, *newptr;
    start = NULL;
    scanf("%c", &c);
    while (c != '\n') {
        d = c-48;
        newptr = createDigit(d);
        if (start == NULL) {
            start = newptr;
            end = start;
        } else {
            end = append(end, newptr);
        }
        scanf("%c", &c);
    }
    return(start);
}

int getNth(struct digit *start, int n){
    int i;
    struct digit *traverse = start;
    for (i=0; i<n; i++) {
        traverse = traverse->next;
    }
    return traverse->num;
}

int getLength(struct digit *start) {
    int i=0;
    struct digit *traverse = start;
    while (traverse != NULL) {
        traverse = traverse->next;
        i++;
    }
    return i;
}

struct digit *reverseNumber(struct digit *start) {
    struct digit *reverse, *end, *newptr;
    int length = getLength(start);
    int i, num;
    
    if (length>0) {
        reverse = createDigit(getNth(start, length-1));
        end = reverse;
        for (i=length-2; i>=0; i--) {
            num = getNth(start, i);
            newptr = createDigit(num);
            end = append(end, newptr);
        }
    } else {
        reverse = NULL;
    }
    return reverse;
}