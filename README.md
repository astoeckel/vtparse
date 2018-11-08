# VTParse - An implementation of Paul Williams' DEC compatible state machine parser

[![Build Status](https://travis-ci.org/astoeckel/vtparse.svg?branch=master)](https://travis-ci.org/astoeckel/vtparse)

Slightly "modernised" version of the [vtparse](https://github.com/haberman/vtparse) library originally created by Joshua Haberman. `vtparse` implements a (mostly) DEC compatible state machine described here: [http://www.vt100.net/emu/dec_ansi_parser](http://www.vt100.net/emu/dec_ansi_parser).

`vtparse` is useful when implementing terminal emulators for the VT100 and newer hardware terminals. The library is written in C89 and has zero dependencies, including the standard library (hence no `malloc`, `memset`, etc.).

The following changes were made to the library:

* **Cleaner code** (subjectively...)<br/>Moved private symbols out of the public header and prefixed all symbols with `vtparse_`. Errors and warnings in with modern Ruby were eliminated. Yielded to my borderline compulsive urge to add useless comments to the code.
* **Build system**<br/>Switched to the Meson build system; generated headers are included in the repository.
* **UTF-8 support**<br/>UTF-8 sequences are passed through in their entirety. Unfortunately, this means that the library is no longer DEC compatible, since 8-bit control characters no longer work. This should be irrelevant in modern applications.
* **Unit tests**<br/>
(*WORK IN PROGRESS*)
* **Return value based parsing**<br/>Eliminated the callback function. `vtparse_parse` returns whenever an event happens. `VTPARSE_ACTION_PUT` and `VTPARSE_ACTION_PRINT` do not trigger an event for each character, instead the corresponding byte range in the input buffer is indicated.


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
