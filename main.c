#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv)
{

	size_t chunkSize = 1;
	unsigned int interval_us = 1000000;

	if (argc > 1) {
		interval_us = 1000000 / atoi(argv[1]);
	}
	if (argc > 2) {
		chunkSize = atoi(argv[2]);
	}

	char buffer[chunkSize];

	while (!feof(stdin)) {
			
		size_t size = fread(buffer, 1, chunkSize, stdin);
		
		fwrite(buffer, size, 1, stdout);
		fflush(stdout);

		usleep(interval_us);

	}

	return 0;
}
