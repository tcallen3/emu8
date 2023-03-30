# NAME

`emu8` - Chip-8 Emulator

# SYNOPSIS

`emu8 [--help] [--config conf.ini] [-s|--scaling scale_factor] [--ipt count] [--eti660] romfile`

# DESCRIPTION

This is a Chip-8 emulator program written in C++17 and intended to run on 
POSIX-compliant systems, conforming to the architecture specified in 
[Cowgod's Documentation](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM).

Graphics, sound, and user input are all handled via the [SDL2 Library](https://wiki.libsdl.org/SDL2/FrontPage).

This implementation is focused on being transparent, correct, and elegant
to a degree. No effort has been made to significantly optimize performance,
given the massive improvement in modern clock speeds relative to the 
nominal Chip-8 clock frequency. Because the architecture was virtual from 
the beginning, timing in `emu8` is largely focused on a reasonable user 
experience, rather than cycle-matching a fuzzily-defined virtual processor.
This timing is controlled by the number of instructions allowed per tick,
and is configurable by the user (see below).

Error conditions (such as invalid instructions or memory faults) trigger
a core dump of the virtual machine memory, along with additional context
written to `stderr`. Further debugging can be accomplished using `gdb(1)` to
examine program state during execution of a given ROM.

# DEPENDENCIES

In order to compile successfully, this code requires the SDL2 libraries to be
present on the system, along with the Boost program_options library.

# OPTIONS

The `--help` option prints a simple summary of the command line arguments and
general usage, amounting to a condensed version of this section.

The `--config` option allows the user to specify an INI-format file that
specifies alternate keybindings from the default. See below for details.

The `-s` or `--scaling` option allows the user to specify a different display 
scaling factor for the in-memory 64x32 video buffer (see below). 

The `--ipt` option allows for user configuration of the maximum number of
instructions per tick. Each tick is 1/60th of a second, and the default
maximum is set to 7 instructions, for an approximate clock rate around
400 Hz. General consensus suggests a value between 400-800 Hz is best.

The `--eti660` option changes the default program starting address to 0x600,
corresponding to the convention for ETI 660 Chip-8 programs. 

For the emulator to work, `romfile` must be a binary file containing valid
Chip-8 machine code. No header or other metadata is required, and the file
will be loaded contiguously in Chip-8 virtual memory at the selected start
location (`0x200` by default, or `0x600` in ETI 660 compatibility mode).

# NOTES

Like most modern Chip-8 emulators, `emu8` ignores the `0x0nnn` (SYS *addr*)
instruction, since this call referred to actual platform-specific system
calls in the original implementation, which no longer correspond to modern
system call APIs. Since ROMs intended for other architectures should not use
these calls, no attempt is made to emulate the original system API. 

Although several versions of the Chip-8 system have been defined, differing
mostly in having some additional instructions and larger video displays,
`emu8` focuses on emulating the original, including none of the Super 
Chip-48 extended instructions. The program also maintains the original 
64x32 pixel display, though the default video mode scales this by a factor
of 10 in each dimension to offer a more legible 640x320 output. 

The keybinding configuration file (FIXME: more here)

# KNOWN ISSUES
