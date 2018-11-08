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
	EXPECT_EQ(VTPARSE_STATE_GROUND, parser.state);
	EXPECT_EQ('d', parser.ch);
	EXPECT_EQ(0, parser.num_params);
	EXPECT_EQ(0, parser.num_intermediate_chars);
	EXPECT_EQ(0, parser.error);
	EXPECT_EQ(buf, parser.data_begin);
	EXPECT_EQ(buf + buf_len, parser.data_end);
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

	EXPECT_EQ(buf_len, vtparse_parse(&parser, buf, buf_len));
	EXPECT_EQ(VTPARSE_ACTION_PRINT, parser.action);
	EXPECT_EQ(VTPARSE_STATE_GROUND, parser.state);
	EXPECT_EQ('.', parser.ch);
	EXPECT_EQ(0, parser.num_params);
	EXPECT_EQ(0, parser.num_intermediate_chars);
	EXPECT_EQ(0, parser.error);
	EXPECT_EQ(buf, parser.data_begin);
	EXPECT_EQ(buf + buf_len, parser.data_end);
}

void test_simple_linebreak() {
	const unsigned char buf[] = "Hello\nWorld";
	const size_t buf_len = sizeof(buf) - 1U;

	vtparse_t parser;
	vtparse_init(&parser);

	EXPECT_EQ(6, vtparse_parse(&parser, buf, buf_len));
	EXPECT_EQ(VTPARSE_ACTION_PRINT, parser.action);
	EXPECT_EQ(VTPARSE_STATE_GROUND, parser.state);
	EXPECT_EQ('\n', parser.ch);
	EXPECT_EQ(0, parser.num_params);
	EXPECT_EQ(0, parser.num_intermediate_chars);
	EXPECT_EQ(0, parser.error);
	EXPECT_EQ(buf, parser.data_begin);
	EXPECT_EQ(buf + 5, parser.data_end);

	EXPECT_EQ(0, vtparse_parse(&parser, buf + 6, buf_len - 6));
	EXPECT_EQ(VTPARSE_ACTION_EXECUTE, parser.action);
	EXPECT_EQ(VTPARSE_STATE_GROUND, parser.state);
	EXPECT_EQ('\n', parser.ch);
	EXPECT_EQ(0, parser.num_params);
	EXPECT_EQ(0, parser.num_intermediate_chars);
	EXPECT_EQ(0, parser.error);
	EXPECT_EQ(buf + 6, parser.data_begin);
	EXPECT_EQ(buf + 6, parser.data_end);

	EXPECT_EQ(5, vtparse_parse(&parser, buf + 6, buf_len - 6));
	EXPECT_EQ(VTPARSE_ACTION_PRINT, parser.action);
	EXPECT_EQ(VTPARSE_STATE_GROUND, parser.state);
	EXPECT_EQ('d', parser.ch);
	EXPECT_EQ(0, parser.num_params);
	EXPECT_EQ(0, parser.num_intermediate_chars);
	EXPECT_EQ(0, parser.error);
	EXPECT_EQ(buf + 6, parser.data_begin);
	EXPECT_EQ(buf + buf_len, parser.data_end);
}

int main() {
	RUN(test_null);
	RUN(test_simple);
	RUN(test_simple_utf8);
	RUN(test_simple_linebreak);
	DONE;
}
