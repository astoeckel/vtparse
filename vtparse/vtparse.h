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

/*
 * This code is based on https://github.com/haberman/vtparse which was released
 * to the public domain.
 */

#ifndef VTPARSE_VTPARSE_H
#define VTPARSE_VTPARSE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Maximum number of characters belonging to a control sequence apart from the
 * parameters.
 */
#define VTPARSE_MAX_INTERMEDIATE_CHARS 2U

/**
 * Maximum number of parameters.
 */
#define VTPARSE_MAX_PARAMS 16U

typedef enum {
	VTPARSE_STATE_CSI_ENTRY = 1,
	VTPARSE_STATE_CSI_IGNORE = 2,
	VTPARSE_STATE_CSI_INTERMEDIATE = 3,
	VTPARSE_STATE_CSI_PARAM = 4,
	VTPARSE_STATE_DCS_ENTRY = 5,
	VTPARSE_STATE_DCS_IGNORE = 6,
	VTPARSE_STATE_DCS_INTERMEDIATE = 7,
	VTPARSE_STATE_DCS_PARAM = 8,
	VTPARSE_STATE_DCS_PASSTHROUGH = 9,
	VTPARSE_STATE_ESCAPE = 10,
	VTPARSE_STATE_ESCAPE_INTERMEDIATE = 11,
	VTPARSE_STATE_GROUND = 12,
	VTPARSE_STATE_OSC_STRING = 13,
	VTPARSE_STATE_SOS_PM_APC_STRING = 14,
} vtparse_state_t;

typedef enum {
	VTPARSE_ACTION_CLEAR = 1,
	VTPARSE_ACTION_COLLECT = 2,
	VTPARSE_ACTION_CSI_DISPATCH = 3,
	VTPARSE_ACTION_ESC_DISPATCH = 4,
	VTPARSE_ACTION_EXECUTE = 5,
	VTPARSE_ACTION_HOOK = 6,
	VTPARSE_ACTION_IGNORE = 7,
	VTPARSE_ACTION_OSC_END = 8,
	VTPARSE_ACTION_OSC_PUT = 9,
	VTPARSE_ACTION_OSC_START = 10,
	VTPARSE_ACTION_PARAM = 11,
	VTPARSE_ACTION_PRINT = 12,
	VTPARSE_ACTION_PUT = 13,
	VTPARSE_ACTION_UNHOOK = 14,
	VTPARSE_ACTION_ERROR = 15,
} vtparse_action_t;

/**
 * Data used internally by vtparse. You should not modify the member variables
 * in this structure.
 */
typedef struct vtparse_private {
	int cycle;
	unsigned char change;
} vtparse_private_t;


typedef struct vtparse {
	/**
	 * Action defined by the control sequence or VTPARSE_ACTION_PRINT if the
	 * given data is just text.
	 */
	vtparse_action_t action;

	/**
	 * Current parser state.
	 */
	vtparse_state_t state;

	/**
	 * Character that is currently being processed.
	 */
	char ch;

	/**
	 * Number of parameters stored in the "params" array.
	 */
	int num_params;

	/**
	 * Number of "intermediate" characters.
	 */
	int num_intermediate_chars;

	/**
	 * If non-zero, indicates that there was an error condition. You should
	 * probably ignore this action.
	 */
	int error;

	/**
	 * Pointer at the first byte that should be printed or put.
	 */
	unsigned char const *data_begin;

	/**
	 * Pointer at the last byte that should be printed or put.
	 */
	unsigned char const *data_end;

	/**
	 * Parameters associated with the current CSI sequence.
	 */
	int params[VTPARSE_MAX_PARAMS];

	/**
	 * Characters occuring after the escape sequence but before any parameters.
	 */
	unsigned char intermediate_chars[VTPARSE_MAX_INTERMEDIATE_CHARS + 1];

	/**
	 * priv_ contains private, implementation specific data. You should not rely
	 * on any fields in this structure in your code.
	 */
	vtparse_private_t priv_;
} vtparse_t;

/**
 * Resets the given parser instance to its initial state.
 *
 * @param parser is a pointer at the vtparse_t instance that should be reset.
 */
void vtparse_init(vtparse_t *parser);

/**
 * Parses the given memory buffer. This function returns either when the end of
 * the buffer has been reached or a command
 */
unsigned int vtparse_parse(vtparse_t *parser, const unsigned char *buf,
                           unsigned int buf_len);

/**
 * Returns a string representation of the given action enum.
 */
const char *vtparse_action_str(vtparse_action_t action);

/**
 * Returns a string representation of the given state enum.
 */
const char *vtparse_state_str(vtparse_state_t state);

#ifdef __cplusplus
}
#endif

#endif /* VTPARSE_VTPARSE_H* */
