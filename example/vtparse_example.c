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

/**
 * @file vtparse_example.c
 *
 * This code is based on https://github.com/haberman/vtparse which was released
 * to the public domain.
 *
 * @author Andreas Stöckel
 */

#include <stdio.h>

#include <vtparse/vtparse.h>

int main() {
	unsigned char buf[1024];
	unsigned int buf_pos, did_read, i;
	unsigned char const *data;
	vtparse_t parser;

	/* Reset/initialize the parser */
	vtparse_init(&parser);
	do {
		/* Try to fill the buffer */
		did_read = fread(buf, 1, sizeof(buf), stdin);
		buf_pos = 0;

		/* Feed the buffer into the parser until we processed all bytes */
		while (buf_pos < did_read) {
			/* Parse the data and advance the cursor position */
			buf_pos += vtparse_parse(&parser, buf + buf_pos, did_read - buf_pos);

			/* The parser returned, so there must be something interesting to
			   report! */
			printf("Received action %s\n", vtparse_action_str(parser.action));

			/* If data_begin/data_end are set, there is a byte sequence that we
			   can process VTPARSE_ACTION_PUT/VTPARSE_ACTION_PRINT. Otherwise
			   print the "parser.ch" for control sequences. */
			if (parser.data_begin != parser.data_end) {
				for (data = parser.data_begin; data < parser.data_end;
				     data++) {
					printf("Char: 0x%02x ('%c')\n", *data, *data);
				}
			} else if (parser.ch != 0) {
				printf("Char: 0x%02x ('%c')\n", parser.ch, parser.ch);
			}

			/* Print any intermediate chars */
			if (parser.num_intermediate_chars > 0) {
				printf("%d Intermediate chars:\n",
				       parser.num_intermediate_chars);
				for (i = 0; i < parser.num_intermediate_chars; i++) {
					printf("  0x%02x ('%c')\n", parser.intermediate_chars[i],
					       parser.intermediate_chars[i]);
				}
			}

			/* Print any parameters */
			if (parser.num_params > 0) {
				printf("%d Parameters:\n", parser.num_params);
				for (i = 0; i < parser.num_params; i++) {
					printf("\t%d\n", parser.params[i]);
				}
			}
			printf("\n");
		}
	} while (did_read > 0); /* Continue until EOF */

	return 0;
}

