# PTM (Programmable Tile Machine)
This program is a virtual machine that tries to emulate many aspects of old 8-bit computer systems and game consoles such as those introduced during the 70's and early 80's, borrowing inspiration from the [Commodore VIC-20](https://en.wikipedia.org/wiki/Commodore_VIC-20), [Mattel Intellivision](https://en.wikipedia.org/wiki/Intellivision), [Atari 800](https://en.wikipedia.org/wiki/Atari_8-bit_family), [Sinclair ZX Spectrum](https://en.wikipedia.org/wiki/ZX_Spectrum), [Tandy/Radio Shack TRS-80](https://en.wikipedia.org/wiki/TRS-80), [MSX](https://en.wikipedia.org/wiki/MSX), among others. It is a so-called "fantasy" computer, as it does not exist in hardware. Compare with other "fantasy" systems or virtual machines such as [PICO-8](https://en.wikipedia.org/wiki/PICO-8), [CHIP-8](https://en.wikipedia.org/wiki/CHIP-8), the [Bitsy](https://ledoux.itch.io/bitsy) engine, [ZZT](https://en.wikipedia.org/wiki/ZZT), and [MegaZeux](https://www.digitalmzx.com/).

The PTM works by running a program written in PTML, which is a low-level interpreted programming language similar to some early Assembly languages along with a few C-like features such as variable typing and arrays. To ease common tasks, the command set is backed by an API that deals with string manipulation, graphics output, sound generation and playback, keyboard input, file I/O, among other stuff.

Since this project is in a very early stage, the machine internal workings as well as the commands and syntax of PTML are subject to very frequent changes, so if you intend to develop applications using PTML targeting the PTM at this moment, please be wary of the fact that your programs most likely will not be 100% compatible with future versions of the interpreter and may require extensive changes to the code or no longer work at all.

For examples of PTM programs, please look inside the folder "Test" where you can find some PTML source code files. If you just want the machine executable (PTM.exe) you can find the latest dev build in the folder "Binaries".

Technologies used in this project:

- [C/C++](https://en.wikipedia.org/wiki/C%2B%2B)
- [Microsoft Visual Studio Community 2017](https://visualstudio.microsoft.com/vs/older-downloads/#visual-studio-2017-and-other-products) (Version 15.9.35)
- [SDL 2.0.8](https://www.libsdl.org/) (x86 DLL)
- [TileGameLib](https://github.com/FernandoAiresCastello/TileGameToolkit) (by [Fernando Aires Castello](https://github.com/FernandoAiresCastello))
- [CppUtils](https://github.com/FernandoAiresCastello/CppUtils) (by [Fernando Aires Castello](https://github.com/FernandoAiresCastello))
