#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "riff.h"
#include "wavfile.h"

int main(int argc,char **argv)
{
	if (argc != 4) {
		printf("Usage: vad-suppress-noise vad_list_file wav_file output_file.\n");
		exit(-1);
	}

	int sample_rate = 16000;
	double time_us_per_sample = 10000000.0 / sample_rate;

	FILE *fp = fopen(argv[1], "r");
	char buf[60];
	if (NULL == fp) {
		printf("Open file failed!\n");
			return -1;
	}
	strbuf_t **input;
	short *s;
	int n;
	input = riff_read_channel(argv[2], &n);

	s = (short *)(input[0]->data);
	if (!s) {
		printf("Read wav failed!");
		exit(-1);
	}

	long int start_time;
	long int end_time;
	char is_speech[6];

	fgets(buf, sizeof(buf), fp);
	while (!feof(fp)) {
		fgets(buf, sizeof(buf), fp);
		sscanf(buf, "%ld %ld %s", &start_time, &end_time, is_speech);

		long int start_index, end_index;
		if (!strcmp(is_speech, "sil")) {
			start_index = start_time / time_us_per_sample;
			end_index = end_time / time_us_per_sample;
			memset(s + 44 + start_index, 0, end_index - start_index + 1);
		}
	}

	if (ferror(fp))
		printf("I/O error when reading!\n");
	else if (!feof(fp))
		printf("End of file reached failed!\n");

	wave_write_file(argv[3], 16000, input[0]->data, input[0]->pos);

	return 0;
}
