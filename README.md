# VTParse - An implementation of Paul Williams' DEC compatible state machine parser

[![Build Status](https://travis-ci.org/astoeckel/vtparse.svg?branch=master)](https://travis-ci.org/astoeckel/vtparse)

Modernized version of Joshua Haberman's [vtparse](https://github.com/haberman/vtparse) library. `vtparse` implements an almost DEC (Digital Equipment Corporation) compatible state machine described by Paul Williams. More information on the state machine can be found here: [http://www.vt100.net/emu/dec_ansi_parser](http://www.vt100.net/emu/dec_ansi_parser).

`vtparse` is useful when implementing terminal emulators for the VT100 and newer hardware terminals manufactured by DEC. The library is written in C89 and has zero dependencies; this includes no dependencies on the standard library (hence no `malloc`, `memset`, etc.). Correspondingly this code should easily run on embedded hardware without an operating system.

## New features

The following changes were made to the original library:

* **Cleaner code** (well, subjectively...)<br/>Moved private symbols out of the public header and prefixed all symbols with `vtparse_`. Errors and warnings with modern Ruby were eliminated. Gave in to my borderline compulsive urge to add useless comments to the code.
* **Build system**<br/>Switched to the Meson build system; generated headers are included in the repository.
* **UTF-8 support**<br/>UTF-8 sequences are passed through in their entirety. Unfortunately, this means that the library is no longer DEC compatible, since 8-bit control characters no longer work. This should be irrelevant in modern applications.
* **Unit tests**<br/>
(*WORK IN PROGRESS*)
* **Return value based parsing**<br/>Eliminated the callback function. `vtparse_parse` returns whenever an event happens. `VTPARSE_ACTION_PUT` and `VTPARSE_ACTION_PRINT` do not trigger an event for each character, instead the corresponding byte range in the input buffer is indicated.
* **Termination of OSC with BEL**<br/>Another DEC-incompatible xterm extension. This feature is e.g. used by `bash` to set the window title.


## How to use

### Initializing the parser

Use `void vtparse_init(vtparse_t *parser)` to initialize and reset the parser. Example:
```C
vtparse_t parser;
vtparse_init(&parser);
```

### Parsing a byte sequence

Use `unsigned int vtparse_parse(vtparse_t *parser, unsigned char *buf, unsigned int buf_len)` to parse a byte sequence. The function will return the number of bytes that were read from the input buffer. This value may be smaller than `buf_len` and even be equal to zero. However, this does not indicated an error, it just means that `vtparse_parse` needs to be called again with the `buf` pointer begin advanced by exactly this value. In fact, you should call `vtparse_parse` with a zero-length input buffer until `vtparse_has_event` returns `false`. For example:
```C
unsigned char buf[1024];
unsigned int buf_pos, did_read;
vtparse_t parser;
vtparse_init(&parser);
do {
	did_read = fread(buf, 1, sizeof(buf), stdin);
	buf_pos = 0;
	while (1) {
		buf_pos += vtparse_parse(&parser, buf + buf_pos, did_read - buf_pos);
		if (!vtparse_has_event(&parser)) {
			break; /* No event to print, exit loop */
		}

		/* We have an event, process */
		/* ... */
	}
} while (did_read > 0); /* Continue until EOF */
```

See `example/vtparse_example.c` for a complete example.

## Building

`vtparse` consists of two `.c` and `.h` files. It should correspondingly be quite straight-forward to integrate with your own project. Additionally, this project supports the Meson build system.

### Manual build

`vtparse` has zero dependencies; it solely consists of the files in the `vtparse` folder. You can for example compile the demo application by running
```
git clone https://github.com/astoeckel/vtparse
cd vtparse
gcc -std=c89 -Wall -o vtparse_example example/vtparse_example.c vtparse/vtparse.c vtparse/vtparse_table.c
echo -e "Hello\nWorld" ./vtparse_example
```

### Using meson

This project supports the Meson build system. To install Meson, run `pip3 install --user meson`. Meson furthermore depends on `ninja`. `ninja` (sometimes called `ninja-build`) should be provided by your Linux distribution. To build the library and run the tests run
```
git clone https://github.com/astoeckel/vtparse
cd vtparse && mkdir build && cd build
meson -Dbuildtype=release ..
ninja
ninja test
```


## Credits and License

This library was written by Joshua Haberman in 2007 with several bugfixes by Julian Scheid. See [https://github.com/haberman/vtparse](https://github.com/haberman/vtparse) for the original repository.

The original library was released to the Public Domain. The changes outlined above are Copyright (c) Andreas Stöckel, 2018. In its entirety, the revised version is made available under the AGPLv3. Please use the original version if these license terms are too restrictive for you.
