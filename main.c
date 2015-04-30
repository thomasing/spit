#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

int showHelp;
size_t chunkSize;
unsigned int interval_us;
char *inputPath = "-";
char *outputPath = "-";

void parseargs(int argc, char **argv);

void print_usage(int argc, char **argv)
{
	printf("Usage: %s [options] [input-file]\n", argv[0]);
}

int main(int argc, char **argv)
{

	// Set defaults
	showHelp = 0;
	chunkSize = 1;
	interval_us = 1000000;

	// Parse arguments
	parseargs(argc, argv);
		
	if (showHelp) {
		print_usage(argc, argv);
		return 0;
	}

	FILE *fpin = stdin;
	if (strcmp(inputPath, "-")) {
		fpin = fopen(inputPath, "r");
		if (!fpin) {
			fprintf(stderr, "Unable to open input file for reading: %s\n", inputPath);
			abort();
		}
	}

	FILE *fpout = stdout;
	if (strcmp(outputPath, "-")) {
		fpout = fopen(outputPath, "w");
		if (!fpout) {
			fprintf(stderr, "Unable to open output file for writing: %s\n", outputPath);
			abort();
		}
	}

	// Start spitting chunks of data
	char buffer[chunkSize];
	while (!feof(fpin)) {
		
		size_t size = fread(buffer, 1, chunkSize, fpin);
		
		fwrite(buffer, size, 1, fpout);
		fflush(fpout);

		usleep(interval_us);

	}

	return 0;
}

void parseargs(int argc, char **argv)
{
	int c;

	for (;;) {
		static struct option long_options[] = 
		{
			// Flags
			{"help",	no_argument, &showHelp, 0},
			// Other
			{"size",	required_argument, 0, 's'},
			{"rate",	required_argument, 0, 'r'},
			{"output",	required_argument, 0, 'o'},
			{0, 0, 0, 0}
		};
		
		int option_index = 0;
		c = getopt_long(argc, argv, "s:r:", long_options, &option_index);

		// End of options?
		if (c == -1) break;

		// Handle option
		switch (c) {
			case 0:
				showHelp = 1;
				break;
			case 's':
				chunkSize = atoi(optarg);
				break;
			case 'r':
				interval_us = 1000000 / atoi(optarg);
				break;
			case 'o':
				outputPath = optarg;
				break;
			case '?':
			default:
				print_usage(argc, argv);
				abort();
		}

	}
	
	if (optind < argc) {
		inputPath = argv[optind];
	}
}
