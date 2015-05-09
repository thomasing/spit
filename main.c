#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>

int showHelp;
size_t chunkSize;
int lineMode;
unsigned int interval_us;
char *inputPath = "-";
char *outputPath = "-";
void parseargs(int argc, char **argv);

void print_usage(int argc, char **argv)
{
	fprintf(stderr, "Usage: %s [options] [input-file]\n", argv[0]);
	fprintf(stderr, "Options: \n");
	fprintf(stderr, " -r <rate>\tRate (per second)\n");
	fprintf(stderr, " -s <size>\tChunk size\n");
	fprintf(stderr, " -l       \tLine mode\n");
}

int main(int argc, char **argv)
{

	// Set defaults
	showHelp = 0;
	chunkSize = 1;
	interval_us = 1000000;
	lineMode = 0;

	// Parse arguments
	parseargs(argc, argv);
	
	// Help?
	if (showHelp) {
		print_usage(argc, argv);
		return 0;
	}

	// Open input file
	FILE *fpin = stdin;
	if (strcmp(inputPath, "-")) {
		fpin = fopen(inputPath, "r");
		if (!fpin) {
			fprintf(stderr, "Unable to open input file for reading: %s\n", inputPath);
			abort();
		}
	}
	
	// Open output file
	FILE *fpout = stdout;
	if (strcmp(outputPath, "-")) {
		fpout = fopen(outputPath, "w");
		if (!fpout) {
			fprintf(stderr, "Unable to open output file for writing: %s\n", outputPath);
			abort();
		}
	}

	if (lineMode) {
		
		// Start spitting lines
		int lines = 0;
		char *buffer[chunkSize];
		size_t n = 0;
		memset(buffer, 0, sizeof(char*)*chunkSize);

		while (!feof(fpin)) {
			
			int size = getline(&buffer[lines], &n, fpin);
			if (size >= 0) {
				++lines;
			}

			if (lines >= chunkSize) {
				int i;
				for (i=0;i<lines;i++) {
					if (buffer[i] == 0) continue;
					fwrite(buffer[i], strlen(buffer[i]), 1, fpout);
					fflush(fpout);
					free(buffer[i]);
					buffer[i] = 0;
				}
					
				usleep(interval_us);
				lines = 0;
			}
		}

		// Print remaining lines
		int i;
		for (i=0;i<lines;i++) {
			if (buffer[i] == 0) continue;
			fwrite(buffer[i], strlen(buffer[i]), 1, fpout);
			fflush(fpout);
			free(buffer[i]);
			buffer[i] = 0;
		}

	} else {
	
		// Start spitting chunks of data
		char buffer[chunkSize];
		while (!feof(fpin)) {
			
			size_t size = fread(buffer, 1, chunkSize, fpin);
			
			fwrite(buffer, size, 1, fpout);
			fflush(fpout);

			usleep(interval_us);
		}
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
			{"line-mode",	no_argument, &lineMode, 'l'},
			// Other
			{"size",	required_argument, 0, 's'},
			{"rate",	required_argument, 0, 'r'},
			{"output",	required_argument, 0, 'o'},
			{0, 0, 0, 0}
		};
		
		int option_index = 0;
		c = getopt_long(argc, argv, "s:r:l", long_options, &option_index);

		// End of options?
		if (c == -1) break;

		// Handle option
		switch (c) {
			case 0:
				showHelp = 1;
				break;
			case 'l':
				lineMode = 1;
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
