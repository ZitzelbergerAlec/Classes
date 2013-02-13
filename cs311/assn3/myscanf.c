#include <stdio.h>
#include <stdlib.h>

#define MAX_WORD_LEN 100	//maximum word length

int main()
{
	int i;
	char buf[MAX_WORD_LEN];
	int result = 0;
	while (result != EOF){
		result = scanf("%[a-zA-Z]", buf); //scan what we want
		printf("%s\n", buf);
		result = scanf("%[^a-zA-Z]", buf); //scan what we don't.
	}
}