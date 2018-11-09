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
 * @file test_vtparse.c
 *
 * The unit-tests collected in this file test several modern usage-scenarios for
 * vtparse, such as handling of UTF-8 encoded strings. These tests do not
 * (fully) ensure that the state machine is actually implemented correctly.
 *
 * @author Andreas Stöckel
 */

#include <foxen/unittest.h>

#include <vtparse/vtparse.h>

void test_null() {
	vtparse_t parser;
	vtparse_init(&parser);
	EXPECT_EQ(0U, vtparse_parse(&parser, NULL, 0));
}

void test_simple() {
	const unsigned char buf[] = "Hello World";
	const size_t buf_len = sizeof(buf) - 1U;

	vtparse_t parser;
	vtparse_init(&parser);

	EXPECT_EQ(buf_len, vtparse_parse(&parser, buf, buf_len));
	EXPECT_EQ(VTPARSE_ACTION_PRINT, parser.action);
	EXPECT_EQ(0, parser.error);
	EXPECT_EQ(buf, parser.data_begin);
	EXPECT_EQ(buf + buf_len, parser.data_end);

	EXPECT_EQ(0U, vtparse_parse(&parser, NULL, 0U));
	EXPECT_FALSE(vtparse_has_event(&parser));
}

void test_simple_utf8() {
	/* Excerpt from iu.wikipedia.org */
	const unsigned char buf[] =
	    "ᐃᓄᐃᑦ ᐅᖃᐅᓯᓕᕆᔨᐅᑉ. ᐃᓄᑦᑎᑑᖅᐳᑦ ᓄᓇᕗᒻᒥ, ᓄᓇᕕᒻᒥ, ᐊᑯᑭᑦᑐᒻᒥ, ᓄᓇᑦᓯᐊᕗᒻᒥ, ᓄᓇᑦᓯᐊᕐᒥ' "
	    "ᐊᒻᒪᓗ ᐊᓛᓯᑲᒥ. ᐃᓄᐃᑦ (ᓄᓇᖃᖅᑳᖅᓯᒪᔪᑦ) ᓄᓇᕗᒥᐅᑦ ᐃᓄᒃᑎᑦᑐᖅ ᐅᖃᐅᓯᕐᖓᐅᑎᖃᕐᒪᑕ. ᐃᓄᐃᑦ "
	    "ᐅᖃᐅᓯᖏᑦ ᐊᔾᔨᒌᙱᑦᑑᑎᐅᒐᓗᐊᖅᑐᑎᒃ ᓄᓇᓖᑦ ᒪᓕᒃᖢᒋᑦ, ᐃᓄᐃᓐᓇᖅᑐᓐ ᐃᓚᐅᓪᓗᓂ, ᐅᖃᐅᓯᕆᔭᐅᔪᖅ "
	    "ᐅᐊᓕᓂᖅᐸᓯᖓᓂᕐᒥᐅᑕᐅᔪᓂ ᓄᓇᕗᒥ. ᐃᓄᐃᓐᓇᖅᑐᓐ ᖃᓕᐅᔮᖅᐸᐃᑎᑐᑦ ᐃᓅᔨᖓᔪᖅᑎᑐᑦ ᑎᑎᕋᐅᓯᖃᖅᑐᑦ "
	    "ᖃᓂᐅᔮᖅᐸᐃᑎᑑᖓᙱᑦᑐᑦ ᑎᑎᕋᐅᓯᖏᑦ. ᐃᓄᒃᑎᑐᑦ/ᐃᓄᐃᓐᓇᖅᑐᓐ ᐅᖃᑎᐅᓯᓖᑦ ᐅᓄᕐᓂᖅᐹᖑᕗᑦ ᓄᓇᕗᒻᒥ. "
	    "70ᐳᓴᓐ ᓄᓇᕗᒻᒥᐅᑦ ᐃᓄᒃᑎᑦᑐᖅ ᐅᖃᐅᓯᕐᖓᐅᑎᖃᕐᒪᑎᑕ. ᐃᓄᐃᑦ ᐅᖃᐅᒥᖏᑦ ᐊᔾᔨᒌᑦᑎᐊᖏᒃᑲᓗᐊᖅᖢᑎᒃ "
	    "ᓄᓇᓖᑦ ᒪᓕᒃᖢᒋᑦ, ᐃᓄᐃᓐᓇᖅᑐᓐ ᐃᓚᐅᓪᓗᓂ, ᐅᖃᐅᓯᕆᔭᐅᔪᖅ ᐅᐊᓕᓂᖅᐸᓯᖓᓂᕐᒥᐅᑕᐅᔪᓂ ᓄᓇᕗᒻᒥ. "
	    "ᐃᓄᐃᓐᓇᖅᑐᓐ ᖃᓕᐅᔮᖅᐸᐃᑎᑐᑦ ᐃᓅᔨᖓᔪᑎᑐᑦ ᑎᑎᕋᐅᓯᖃᐃᔾᔪᔪᒡ ᑕᖅᓴᓕᖅᐹᖅᑐᑦ ᐃᓄᒃᑎᑑᖏᑦᑐᐴ.";
	const size_t buf_len = sizeof(buf) - 1U;

	vtparse_t parser;
	vtparse_init(&parser);

	EXPECT_FALSE(vtparse_has_event(&parser));
	EXPECT_EQ(buf_len, vtparse_parse(&parser, buf, buf_len));
	EXPECT_TRUE(vtparse_has_event(&parser));
	EXPECT_EQ(VTPARSE_ACTION_PRINT, parser.action);
	EXPECT_EQ(0, parser.error);
	EXPECT_EQ(buf, parser.data_begin);
	EXPECT_EQ(buf + buf_len, parser.data_end);

	EXPECT_EQ(0U, vtparse_parse(&parser, NULL, 0U));
	EXPECT_FALSE(vtparse_has_event(&parser));
}

void test_linebreak() {
	const unsigned char buf[] = "\n";
	const size_t buf_len = sizeof(buf) - 1U;

	vtparse_t parser;
	vtparse_init(&parser);

	EXPECT_FALSE(vtparse_has_event(&parser));
	EXPECT_EQ(1, vtparse_parse(&parser, buf, buf_len));
	EXPECT_TRUE(vtparse_has_event(&parser));
	EXPECT_EQ(VTPARSE_ACTION_EXECUTE, parser.action);
	EXPECT_EQ('\n', parser.ch);
	EXPECT_EQ(0, parser.error);

	EXPECT_EQ(0U, vtparse_parse(&parser, NULL, 0U));
	EXPECT_FALSE(vtparse_has_event(&parser));
}

void test_simple_linebreak() {
	const unsigned char buf[] = "Hello\nWorld";
	const size_t buf_len = sizeof(buf) - 1U;

	vtparse_t parser;
	vtparse_init(&parser);

	EXPECT_FALSE(vtparse_has_event(&parser));
	EXPECT_EQ(6, vtparse_parse(&parser, buf, buf_len));
	EXPECT_TRUE(vtparse_has_event(&parser));
	EXPECT_EQ(VTPARSE_ACTION_PRINT, parser.action);
	EXPECT_EQ(0, parser.error);
	EXPECT_EQ(buf, parser.data_begin);
	EXPECT_EQ(buf + 5, parser.data_end);

	EXPECT_EQ(0, vtparse_parse(&parser, buf + 6, buf_len - 6));
	EXPECT_TRUE(vtparse_has_event(&parser));
	EXPECT_EQ(VTPARSE_ACTION_EXECUTE, parser.action);
	EXPECT_EQ('\n', parser.ch);
	EXPECT_EQ(0, parser.error);

	EXPECT_EQ(0U, vtparse_parse(&parser, NULL, 0U));
	EXPECT_FALSE(vtparse_has_event(&parser));

	EXPECT_EQ(5, vtparse_parse(&parser, buf + 6, buf_len - 6));
	EXPECT_TRUE(vtparse_has_event(&parser));
	EXPECT_EQ(VTPARSE_ACTION_PRINT, parser.action);
	EXPECT_EQ(0, parser.error);
	EXPECT_EQ(buf + 6, parser.data_begin);
	EXPECT_EQ(buf + buf_len, parser.data_end);

	EXPECT_EQ(0U, vtparse_parse(&parser, NULL, 0U));
	EXPECT_FALSE(vtparse_has_event(&parser));
}

void test_end_linebreak() {
	const unsigned char buf[] = "A\n";
	const size_t buf_len = sizeof(buf) - 1U;

	vtparse_t parser;
	vtparse_init(&parser);

	EXPECT_FALSE(vtparse_has_event(&parser));
	EXPECT_EQ(2, vtparse_parse(&parser, buf, buf_len));
	EXPECT_TRUE(vtparse_has_event(&parser));
	EXPECT_EQ(VTPARSE_ACTION_PRINT, parser.action);
	EXPECT_EQ('\n', parser.ch);
	EXPECT_EQ(0, parser.error);
	EXPECT_EQ(buf, parser.data_begin);
	EXPECT_EQ(buf + 1, parser.data_end);

	EXPECT_EQ(0, vtparse_parse(&parser, buf + 2, buf_len - 2));
	EXPECT_TRUE(vtparse_has_event(&parser));
	EXPECT_EQ(VTPARSE_ACTION_EXECUTE, parser.action);
	EXPECT_EQ('\n', parser.ch);
	EXPECT_EQ(0, parser.num_params);
	EXPECT_EQ(0, parser.num_intermediate_chars);
	EXPECT_EQ(0, parser.error);

	EXPECT_EQ(0U, vtparse_parse(&parser, NULL, 0U));
	EXPECT_FALSE(vtparse_has_event(&parser));
}

void test_standalone_csi() {
	const unsigned char buf[] = "\e[1m";
	const size_t buf_len = sizeof(buf) - 1U;

	vtparse_t parser;
	vtparse_init(&parser);

	EXPECT_FALSE(vtparse_has_event(&parser));
	EXPECT_EQ(4, vtparse_parse(&parser, buf, buf_len));
	EXPECT_TRUE(vtparse_has_event(&parser));
	EXPECT_EQ(VTPARSE_ACTION_CSI_DISPATCH, parser.action);
	EXPECT_EQ('m', parser.ch);
	EXPECT_EQ(1, parser.num_params);
	EXPECT_EQ(1, parser.params[0]);
	EXPECT_EQ(0, parser.num_intermediate_chars);
	EXPECT_EQ(0, parser.error);

	EXPECT_EQ(0U, vtparse_parse(&parser, NULL, 0U));
	EXPECT_FALSE(vtparse_has_event(&parser));
}

void test_multiparam_csi() {
	const unsigned char buf[] = "\e[1m\e[38;2;255;128;255mPINK\e[0m";
	const size_t buf_len = sizeof(buf) - 1U;

	vtparse_t parser;
	vtparse_init(&parser);

	EXPECT_FALSE(vtparse_has_event(&parser));
	EXPECT_EQ(4, vtparse_parse(&parser, buf, buf_len));
	EXPECT_TRUE(vtparse_has_event(&parser));
	EXPECT_EQ(VTPARSE_ACTION_CSI_DISPATCH, parser.action);
	EXPECT_EQ('m', parser.ch);
	EXPECT_EQ(1, parser.num_params);
	EXPECT_EQ(1, parser.params[0]);
	EXPECT_EQ(0, parser.num_intermediate_chars);
	EXPECT_EQ(0, parser.error);

	EXPECT_EQ(19, vtparse_parse(&parser, buf + 4, buf_len - 4));
	EXPECT_TRUE(vtparse_has_event(&parser));
	EXPECT_EQ(VTPARSE_ACTION_CSI_DISPATCH, parser.action);
	EXPECT_EQ('m', parser.ch);
	EXPECT_EQ(5, parser.num_params);
	EXPECT_EQ(38, parser.params[0]);
	EXPECT_EQ(2, parser.params[1]);
	EXPECT_EQ(255, parser.params[2]);
	EXPECT_EQ(128, parser.params[3]);
	EXPECT_EQ(255, parser.params[4]);
	EXPECT_EQ(0, parser.num_intermediate_chars);
	EXPECT_EQ(0, parser.error);

	EXPECT_EQ(8, vtparse_parse(&parser, buf + 23, buf_len - 23));
	EXPECT_TRUE(vtparse_has_event(&parser));
	EXPECT_EQ(VTPARSE_ACTION_PRINT, parser.action);
	EXPECT_EQ(0, parser.error);
	EXPECT_EQ(buf + 23, parser.data_begin);
	EXPECT_EQ(buf + 27, parser.data_end);

	EXPECT_EQ(0U, vtparse_parse(&parser, NULL, 0U));
	EXPECT_TRUE(vtparse_has_event(&parser));
	EXPECT_EQ(VTPARSE_ACTION_CSI_DISPATCH, parser.action);
	EXPECT_EQ('m', parser.ch);
	EXPECT_EQ(1, parser.num_params);
	EXPECT_EQ(0, parser.params[0]);
	EXPECT_EQ(0, parser.num_intermediate_chars);
	EXPECT_EQ(0, parser.error);

	EXPECT_EQ(0U, vtparse_parse(&parser, NULL, 0U));
	EXPECT_FALSE(vtparse_has_event(&parser));
}

void test_esc_1() {
	const unsigned char buf[] = "\eN";
	const size_t buf_len = sizeof(buf) - 1U;

	vtparse_t parser;
	vtparse_init(&parser);

	EXPECT_FALSE(vtparse_has_event(&parser));
	EXPECT_EQ(2, vtparse_parse(&parser, buf, buf_len));
	EXPECT_TRUE(vtparse_has_event(&parser));
	EXPECT_EQ(VTPARSE_ACTION_ESC_DISPATCH, parser.action);
	EXPECT_EQ('N', parser.ch);
	EXPECT_EQ(0, parser.num_intermediate_chars);
	EXPECT_EQ(0, parser.error);

	EXPECT_EQ(0U, vtparse_parse(&parser, NULL, 0U));
	EXPECT_FALSE(vtparse_has_event(&parser));
}

void test_esc_2() {
	const unsigned char buf[] = "A\eOB";
	const size_t buf_len = sizeof(buf) - 1U;

	vtparse_t parser;
	vtparse_init(&parser);

	EXPECT_FALSE(vtparse_has_event(&parser));
	EXPECT_EQ(3, vtparse_parse(&parser, buf, buf_len));
	EXPECT_TRUE(vtparse_has_event(&parser));
	EXPECT_EQ(VTPARSE_ACTION_PRINT, parser.action);
	EXPECT_EQ(0, parser.error);
	EXPECT_EQ(buf, parser.data_begin);
	EXPECT_EQ(buf + 1, parser.data_end);

	EXPECT_EQ(0, vtparse_parse(&parser, buf + 3, buf_len - 3));
	EXPECT_TRUE(vtparse_has_event(&parser));
	EXPECT_EQ(VTPARSE_ACTION_ESC_DISPATCH, parser.action);
	EXPECT_EQ('O', parser.ch);
	EXPECT_EQ(0, parser.num_intermediate_chars);
	EXPECT_EQ(0, parser.error);

	EXPECT_EQ(1, vtparse_parse(&parser, buf + 3, buf_len - 3));
	EXPECT_TRUE(vtparse_has_event(&parser));
	EXPECT_EQ(VTPARSE_ACTION_PRINT, parser.action);
	EXPECT_EQ(0, parser.error);
	EXPECT_EQ(buf + 3, parser.data_begin);
	EXPECT_EQ(buf + 4, parser.data_end);

	EXPECT_EQ(0U, vtparse_parse(&parser, NULL, 0U));
	EXPECT_FALSE(vtparse_has_event(&parser));
}

void test_osc_1() {
	const unsigned char buf[] = "\e]0;Hallo\e\\Welt";
	const size_t buf_len = sizeof(buf) - 1U;

	vtparse_t parser;
	vtparse_init(&parser);

	EXPECT_EQ(2, vtparse_parse(&parser, buf, buf_len));
	EXPECT_TRUE(vtparse_has_event(&parser));
	EXPECT_EQ(VTPARSE_ACTION_OSC_START, parser.action);

	EXPECT_EQ(8, vtparse_parse(&parser, buf + 2, buf_len - 2));
	EXPECT_TRUE(vtparse_has_event(&parser));
	EXPECT_EQ(VTPARSE_ACTION_OSC_PUT, parser.action);
	EXPECT_EQ(0, parser.error);
	EXPECT_EQ(buf + 2, parser.data_begin);
	EXPECT_EQ(buf + 9, parser.data_end);

	EXPECT_EQ(0, vtparse_parse(&parser, buf + 10, buf_len - 10));
	EXPECT_TRUE(vtparse_has_event(&parser));
	EXPECT_EQ(VTPARSE_ACTION_OSC_END, parser.action);
	EXPECT_EQ(0, parser.error);

	EXPECT_EQ(1, vtparse_parse(&parser, buf + 10, buf_len - 10));
	EXPECT_TRUE(vtparse_has_event(&parser));
	EXPECT_EQ(VTPARSE_ACTION_ESC_DISPATCH, parser.action);
	EXPECT_EQ(0, parser.num_intermediate_chars);
	EXPECT_EQ(0, parser.error);

	EXPECT_EQ(4, vtparse_parse(&parser, buf + 11, buf_len - 11));
	EXPECT_TRUE(vtparse_has_event(&parser));
	EXPECT_EQ(VTPARSE_ACTION_PRINT, parser.action);
	EXPECT_EQ(0, parser.error);
	EXPECT_EQ(buf + 11, parser.data_begin);
	EXPECT_EQ(buf + 15, parser.data_end);

	EXPECT_EQ(0U, vtparse_parse(&parser, NULL, 0U));
	EXPECT_FALSE(vtparse_has_event(&parser));
}

void test_osc_2() {
	const unsigned char buf[] = "\e]0;Hallo\aWelt"; /* Termination using BEL */
	const size_t buf_len = sizeof(buf) - 1U;

	vtparse_t parser;
	vtparse_init(&parser);

	EXPECT_EQ(2, vtparse_parse(&parser, buf, buf_len));
	EXPECT_TRUE(vtparse_has_event(&parser));
	EXPECT_EQ(VTPARSE_ACTION_OSC_START, parser.action);

	EXPECT_EQ(8, vtparse_parse(&parser, buf + 2, buf_len - 2));
	EXPECT_TRUE(vtparse_has_event(&parser));
	EXPECT_EQ(VTPARSE_ACTION_OSC_PUT, parser.action);
	EXPECT_EQ(0, parser.error);
	EXPECT_EQ(buf + 2, parser.data_begin);
	EXPECT_EQ(buf + 9, parser.data_end);

	EXPECT_EQ(0, vtparse_parse(&parser, buf + 10, buf_len - 10));
	EXPECT_TRUE(vtparse_has_event(&parser));
	EXPECT_EQ(VTPARSE_ACTION_OSC_END, parser.action);
	EXPECT_EQ(0, parser.error);

	EXPECT_EQ(4, vtparse_parse(&parser, buf + 10, buf_len - 10));
	EXPECT_TRUE(vtparse_has_event(&parser));
	EXPECT_EQ(VTPARSE_ACTION_PRINT, parser.action);
	EXPECT_EQ(0, parser.error);
	EXPECT_EQ(buf + 10, parser.data_begin);
	EXPECT_EQ(buf + 14, parser.data_end);

	EXPECT_EQ(0U, vtparse_parse(&parser, NULL, 0U));
	EXPECT_FALSE(vtparse_has_event(&parser));
}

void test_intermediate_csi_1() {
	const unsigned char buf[] = "\e[?25hABC";
	const size_t buf_len = sizeof(buf) - 1U;

	vtparse_t parser;
	vtparse_init(&parser);

	EXPECT_FALSE(vtparse_has_event(&parser));
	EXPECT_EQ(6, vtparse_parse(&parser, buf, buf_len));
	EXPECT_TRUE(vtparse_has_event(&parser));
	EXPECT_EQ(VTPARSE_ACTION_CSI_DISPATCH, parser.action);
	EXPECT_EQ('h', parser.ch);
	EXPECT_EQ(1, parser.num_params);
	EXPECT_EQ(25, parser.params[0]);
	EXPECT_EQ(1, parser.num_intermediate_chars);
	EXPECT_EQ('?', parser.intermediate_chars[0]);
	EXPECT_EQ(0, parser.error);

	EXPECT_EQ(3, vtparse_parse(&parser, buf + 6, buf_len - 6));
	EXPECT_TRUE(vtparse_has_event(&parser));
	EXPECT_EQ(VTPARSE_ACTION_PRINT, parser.action);
	EXPECT_EQ(0, parser.error);
	EXPECT_EQ(buf + 6, parser.data_begin);
	EXPECT_EQ(buf + 9, parser.data_end);

	EXPECT_EQ(0U, vtparse_parse(&parser, NULL, 0U));
	EXPECT_FALSE(vtparse_has_event(&parser));
}

void test_intermediate_csi_2() {
	const unsigned char buf[] = "\e[??25hABC";
	const size_t buf_len = sizeof(buf) - 1U;

	vtparse_t parser;
	vtparse_init(&parser);

	EXPECT_FALSE(vtparse_has_event(&parser));
	EXPECT_EQ(10, vtparse_parse(&parser, buf, buf_len));
	EXPECT_TRUE(vtparse_has_event(&parser));
	EXPECT_EQ(VTPARSE_ACTION_PRINT, parser.action);
	EXPECT_EQ(0, parser.error);
	EXPECT_EQ(buf + 7, parser.data_begin);
	EXPECT_EQ(buf + 10, parser.data_end);

	EXPECT_EQ(0U, vtparse_parse(&parser, NULL, 0U));
	EXPECT_FALSE(vtparse_has_event(&parser));
}

int main() {
	RUN(test_null);
	RUN(test_simple);
	RUN(test_simple_utf8);
	RUN(test_linebreak);
	RUN(test_simple_linebreak);
	RUN(test_end_linebreak);
	RUN(test_standalone_csi);
	RUN(test_multiparam_csi);
	RUN(test_esc_1);
	RUN(test_esc_2);
	RUN(test_osc_1);
	RUN(test_osc_2);
	RUN(test_intermediate_csi_1);
	RUN(test_intermediate_csi_2);
	DONE;
}
