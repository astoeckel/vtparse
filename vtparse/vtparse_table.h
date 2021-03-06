/******************************************************************************
 * Note: This file was automatically generated. Please execute                *
 * tables/vtparse_gen_c_tables.rb to re-generate this file.                   *
 ******************************************************************************/

#ifndef VTPARSE_VTPARSE_TABLE_H
#define VTPARSE_VTPARSE_TABLE_H

#ifdef __cplusplus
extern "C" {
#endif

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

#ifndef VTPARSE_VTPARSE_H

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


#endif

typedef unsigned char vtparse_state_change_t;

extern vtparse_state_change_t STATE_TABLE[14][256];
extern vtparse_action_t ENTRY_ACTIONS[14];
extern vtparse_action_t EXIT_ACTIONS[14];
extern char *ACTION_NAMES[16];
extern char *STATE_NAMES[15];

#ifdef __cplusplus
}
#endif

#endif /* VTPARSE_VTPARSE_TABLE_H */

