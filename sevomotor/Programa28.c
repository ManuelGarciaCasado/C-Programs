
#include <stdio.h>
#include <string.h>

char palabra[20];
char i = 0;
char j = 0;
char *p;
int main(){
	printf("Escribe una palabra:\n");
	scanf("%s", palabra);
	while(palabra[i] != '\0'){
		if(j%2 ==0){
		  palabra[i] = toupper(palabra[i]);
		  printf("%c", palabra[i]);
		}
		else{
          printf("%c", palabra[i]);
		}
		i++;
		j++;
	}
	printf("\n");
}