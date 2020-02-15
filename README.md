# YAGBS - Yet Another Gameboy Snake

This is a simple snake game written in C using the GBDK toolchain for Gameboy.

## Getting Started

### Prerequisites

GBDK is the only pre-requisite to build this project. Make sure you clone this
project to include the GBDK submodule
```
$ git clone --recurse-submodules https://github.com/michaelmcallister/yagbs.git
```

For Linux, you can install GBDK with the following commands
```bash
$ cd gbdk
$ make
$ make install
```

### Compiling (Linux)

Compiling is easy once GBDK is installed:

- run `make clean` from the root directory to remove the ROM shipped with the repo
- run `make` to build from source

The game will be written to the `dist/` folder

```bash
ls -Gg dist/
total 64
-rwxrwxrwx 1 65536 Feb 15 11:12 snake.gb
```

## Built With

* [GBDK](http://gbdk.sourceforge.net/) - Gameboy toolchain (compiler, assembler, linker, libraries)
* [GBTD](http://www.devrs.com/gb/hmgd/gbtd.html) - Gameboy Tile Designer 
* [GBMB](http://www.devrs.com/gb/hmgd/gbmb.html) - Gameboy Map Designer


## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* [GBDK](http://gbdk.sourceforge.net/) for making a library that means I don't have to write ASM.