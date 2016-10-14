#include <stdio.h>

int main(int argc, char** argv) {
	int i;
	printf("Got %i args:\n", argc);
	for (i = 0; i < argc; i++)
		printf("\t- %s\n", argv[i]);
	return 0;
}

