# DMENU for Windows

A [dmenu](https://tools.suckless.org/dmenu/) clone for the Windows
operating system written with pure win32 API.

This project is based on [wmenu](https://github.com/LinArcX/wmenu), but
with these key differences:

- It presents the same interface as dmenu, for compatibility with
  existing dmenu scripts.
- It builds with MinGW, making it easier to compile on Windows systems.ç
- Cleaner codebase with fewer features, focusing on core functionality.
- Ignores unsupported `dmenu` options to maintain compatibility.

Check out [Busybox for Windows](https://frippery.org/busybox/) for a
a shell for running dmenu scripts.

	#!/bin/sh -e
	CHOICE="$(dmenu -i -p "Choose an option:" <<-EOF
	Option 1
	Option 2
	Option 3
	EOF
	)"
	case "$CHOICE" in
	    "Option 1") echo "You chose option 1" ;;
	    "Option 2") echo "You chose option 2" ;;
	    "Option 3") echo "You chose option 3" ;;
	    *) echo "No valid option chosen" ;;
	esac

## Building

You need a MinGW toolchain, read the makefile for details.

    make
    make install

## TODO

- [ ] Implement the option for background/foreground colors.
- [ ] Implement the position options, such as `-b` for bottom placement.
- [ ] Implement monitor selection options.

## Collaboration

Feel free to open bug reports and feature/pull requests.

More software like this here:

1. [https://harkadev.com/prj/](https://harkadev.com/prj/)
2. [https://devreal.org](https://devreal.org)
