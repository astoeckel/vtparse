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
 * @file vtparse.c
 *
 * Code implementing the actual parser state machine by wrapping the
 * auto-generated state-transition tables. This code is based on
 * https://github.com/haberman/vtparse which was released to the public domain.
 *
 * @author Joshua Haberman
 * @author Andreas Stöckel
 */

#include <vtparse/vtparse.h>
#include <vtparse/vtparse_table.h>

/******************************************************************************
 * Private implementation details                                             *
 ******************************************************************************/

typedef enum vtparse_cycle {
	VTPARSE_CYCLE_READ_CHAR = 0,
	VTPARSE_CYCLE_EXIT_ACTION = 1,
	VTPARSE_CYCLE_ACTION = 2,
	VTPARSE_CYCLE_ENTRY_ACTION = 3,
	VTPARSE_CYCLE_ENTRY_ACTION_DONE = 4,
	VTPARSE_CYCLE_READ_CHAR_DONE = 5
} vtparse_cycle_t;

/**
 * Extracts the action stored in the vtparse.state variable.
 */
#define ACTION(state_change) ((state_change & 0x0FU) >> 0)

/**
 * Extracts the state stored in the vtparse.state variable.
 */
#define STATE(state_change) ((state_change & 0xF0U) >> 4)

/**
 * Decide whether to pass an action to the user or to handle it internally.
 */
static int vtparse_execute_action(vtparse_t *parser, vtparse_action_t action,
                                  char ch) {
	switch (action) {
		case VTPARSE_ACTION_PRINT:
		case VTPARSE_ACTION_PUT:
		case VTPARSE_ACTION_OSC_PUT:
			parser->data_end++;
			parser->action = action;
			return 0;

		case VTPARSE_ACTION_EXECUTE:
		case VTPARSE_ACTION_HOOK:
		case VTPARSE_ACTION_OSC_START:
		case VTPARSE_ACTION_OSC_END:
		case VTPARSE_ACTION_UNHOOK:
		case VTPARSE_ACTION_CSI_DISPATCH:
		case VTPARSE_ACTION_ESC_DISPATCH:
			return 1;

		case VTPARSE_ACTION_IGNORE:
			return 0;

		case VTPARSE_ACTION_COLLECT:
			/* Append the character to the intermediate params */
			if (parser->num_intermediate_chars + 1 >
			    VTPARSE_MAX_INTERMEDIATE_CHARS) {
				parser->error = 1;
			} else {
				parser->intermediate_chars[parser->num_intermediate_chars++] =
				    ch;
			}
			return 0;

		case VTPARSE_ACTION_PARAM:
			/* process the param character */
			if (ch == ';') {
				if (parser->num_params + 1 < VTPARSE_MAX_PARAMS) {
					parser->num_params += 1;
					parser->params[parser->num_params - 1] = 0;
				} else {
					parser->error = 1;
				}
			} else if (!parser->error) {
				/* the character is a digit */
				int current_param;

				if (parser->num_params == 0) {
					parser->num_params = 1;
					parser->params[0] = 0;
				}

				current_param = parser->num_params - 1;
				parser->params[current_param] *= 10;
				parser->params[current_param] += (ch - '0');
			}
			return 0;

		case VTPARSE_ACTION_CLEAR:
			parser->num_intermediate_chars = 0;
			parser->num_params = 0;
			parser->error = 0;
			return 0;

		default:
			parser->error = 1;
			return 0;
	}
}

static int vtparse_handle_action(vtparse_t *parser, vtparse_action_t action) {
	vtparse_private_t *priv = &parser->priv_;
	int must_return = 0;

	if (action && vtparse_execute_action(parser, action, parser->ch)) {
		/* Before we can return the actual action, we need to return some
		   previously buffered data */
		if (parser->data_end - parser->data_begin) {
			return 1; /* Need to return to the user */
		}

		/* Return this action to the user */
		parser->action = action;
		must_return = 1;
	}

	/* Go to the next cycle */
	priv->cycle = (!STATE(priv->change)) ? VTPARSE_CYCLE_READ_CHAR_DONE
	                                     : (priv->cycle + 1);
	return must_return;
}

/******************************************************************************
 * Public C API                                                               *
 ******************************************************************************/

void vtparse_init(vtparse_t *parser) {
	unsigned int i;

	/* Reset all non-array fields */
	parser->action = 0;
	parser->ch = 0;
	parser->num_params = 0;
	parser->num_intermediate_chars = 0;
	parser->error = 0;
	parser->data_begin = 0;
	parser->data_end = 0;

	/* Reset all arrays */
	for (i = 0; i < VTPARSE_MAX_PARAMS; i++) {
		parser->params[i] = 0;
	}
	for (i = 0; i < VTPARSE_MAX_INTERMEDIATE_CHARS; i++) {
		parser->intermediate_chars[i] = 0;
	}

	/* Reset the private data */
	parser->priv_.change = 0;
	parser->priv_.cycle = VTPARSE_CYCLE_READ_CHAR;
	parser->priv_.state = VTPARSE_STATE_GROUND;
}

unsigned int vtparse_parse(vtparse_t *parser, const unsigned char *buf,
                           unsigned int buf_len) {
	/* Number of characters read from the input */
	unsigned char ch;
	int n_read = 0;

	/* Convenient reference at the private data */
	vtparse_private_t *priv = &parser->priv_;

	/* Reset the "data" pointers in the parser instance to indicate that we have
	   not processed any characters so far. */
	parser->data_begin = buf;
	parser->data_end = buf;

	while (1) {
		switch (priv->cycle) {
			/* Read the next character from the input and determine whether the
			   state changes. Abort if we are at the end of the input buffer. */
			case VTPARSE_CYCLE_READ_CHAR: {
				/* Return if we have reached the end of the buffer */
				if (n_read >= buf_len) {
					/* Switch to a different state to indicate that we actually
					   have data waiting for the user */
					if (n_read) {
						priv->cycle = VTPARSE_CYCLE_READ_CHAR_DONE;
					}
					return n_read;
				}

				/* Read the next character */
				parser->ch = ch = buf[n_read++];

				/* Determine which state change is required */
				priv->change = STATE_TABLE[priv->state - 1][ch];
				if (STATE(priv->change)) {
					priv->cycle = VTPARSE_CYCLE_EXIT_ACTION;
				} else {
					priv->cycle = VTPARSE_CYCLE_ACTION;
				}
				break;
			}

			/* Execute the exit action for the last state the parser was in */
			case VTPARSE_CYCLE_EXIT_ACTION:
				if (vtparse_handle_action(parser,
				                          EXIT_ACTIONS[priv->state - 1])) {
					return n_read; /* Return to the user if necessary */
				}
				break;

			/* Execute the action associated with the character read above */
			case VTPARSE_CYCLE_ACTION:
				if (vtparse_handle_action(parser, ACTION(priv->change))) {
					return n_read; /* Return to the user if necessary */
				}
				break;

			/* Execute the entry action if there was a state change */
			case VTPARSE_CYCLE_ENTRY_ACTION:
				if (vtparse_handle_action(
				        parser, ENTRY_ACTIONS[STATE(priv->change) - 1])) {
					return n_read; /* Return to the user if necessary */
				}
				break;

			/* Transition to the next state */
			case VTPARSE_CYCLE_ENTRY_ACTION_DONE:
				priv->cycle = VTPARSE_CYCLE_READ_CHAR;
				priv->state = STATE(priv->change);
				break;

			/* Transition back to the READ_CHAR state */
			case VTPARSE_CYCLE_READ_CHAR_DONE:
				priv->cycle = VTPARSE_CYCLE_READ_CHAR;
				break;
		}
	}
	return n_read;
}

int vtparse_has_event(const vtparse_t *parser) {
	return (!parser->error) && parser->priv_.cycle != VTPARSE_CYCLE_READ_CHAR;
}

const char *vtparse_action_str(vtparse_action_t action) {
	return ACTION_NAMES[action];
}

