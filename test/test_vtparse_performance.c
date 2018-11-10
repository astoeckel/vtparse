/*
 * VTParse -- An implementation of Paul Williams' DEC compatible state machine
 *
 * Copyright (C) 2007  Joshua Haberman <joshua@reverberate.org>
 * Copyright (C) 2018  Andreas Stöckel
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#define _POSIX_C_SOURCE 199309L
#include <time.h>

#include <stdio.h>
#include <stdlib.h>

#include <foxen/unittest.h>
#include <vtparse/vtparse.c>

static const int N_REPEAT = 1000;

static long long int microtime() {
	struct timespec tp;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tp);
	return tp.tv_sec * 1000 * 1000 + tp.tv_nsec / 1000;
}

int main(int argc, char *argv[]) {
	int i, j;
	unsigned char **file_buffers;
	unsigned int *file_buffer_sizes;
	FILE *f;

	/* Check the arguments */
	if (argc <= 1) {
		fprintf(stderr,
		        "Usage ./test_vtparse_performance <INPUT FILE 1> ... <INPUT "
		        "FILE N>\n");
		return 1;
	}

	/* Read the specified files to memory */
	file_buffers = calloc(sizeof(char *), argc - 1);
	file_buffer_sizes = calloc(sizeof(*file_buffer_sizes), argc - 1);
	FX_PRINT_ARROW("Reading input files to memory\n");
	for (i = 0; i < argc - 1; i++) {
		fprintf(stdout,
		        "      "
		        "Reading \"%s\"...\n",
		        argv[i + 1]);
		file_buffers[i] = 0;
		f = fopen(argv[i + 1], "r");
		if (!f) {
			FX_PRINT_ERR("Error while opening file!\n");
			return 1;
		}
		while (!feof(f)) {
			/* Resize the target buffer */
			const size_t old_buf_size = file_buffer_sizes[i];
			const size_t add_buf_size = old_buf_size ? old_buf_size : 4096U;
			file_buffer_sizes[i] = old_buf_size + add_buf_size;
			file_buffers[i] = realloc(file_buffers[i], file_buffer_sizes[i]);

			/* Read the new number of bytes into the buffer */
			file_buffer_sizes[i] =
			    old_buf_size +
			    fread(file_buffers[i] + old_buf_size, 1, add_buf_size, f);
			file_buffers[i] = realloc(file_buffers[i], file_buffer_sizes[i]);
		}
		fclose(f);
	}
	FX_PRINT_OK("Reading input files to memory\n");

	FX_PRINT_ARROW("Parsing files\n");
	for (i = 0; i < argc - 1; i++) {
		/* Pass the file n_repeat times through vtparse */
		long long int t0, t1, tsum = 0, nsmul = 1000;
		long long int buf_pos;
		for (j = 0; j < N_REPEAT; j++) {
			fprintf(stderr,
			        "\r"
			        "      "
			        "Parsing \"%s\" (iteration %04d/%04d)...",
			        argv[i + 1], j + 1, N_REPEAT);
			t0 = microtime();
			unsigned char *buf = file_buffers[i];
			unsigned int buf_len = file_buffer_sizes[i];
			buf_pos = 0;
			volatile vtparse_t parser_volatile;
			vtparse_t parser;
			vtparse_init(&parser);

			while (1) {
				buf_pos +=
				    vtparse_parse(&parser, buf + buf_pos, buf_len - buf_pos);
				if (!vtparse_has_event(&parser)) {
					break;
				}
				/* Look at the parsed content to prevent the parser from being
				   optimized away (when using LTO) */
				parser_volatile = parser;
				(void)parser_volatile;
			}
			t1 = microtime();
			tsum += (t1 - t0);
		}
		fprintf(stdout,
		        "\r"
		        "      "
		        "Parsing \"%s\" (%lld bytes) took %dµs per iteration (%dns "
		        "per byte)\n",
		        argv[i + 1], buf_pos, (int)(tsum / (long long int)N_REPEAT),
		        (int)((tsum * nsmul) / (long long int)(N_REPEAT * buf_pos)));
	}
	FX_PRINT_OK("Done parsing files.\n");

	/* Deallocate all dynamically allocated memory */
	for (i = 0; i < argc - 1; i++) {
		free(file_buffers[i]);
	}
	free(file_buffers);
	free(file_buffer_sizes);
	return 0;
}
