# NBT TUI Editor

A simple terminal-based editor for Minecraft's NBT (.dat) files written in C++.

## Overview

This is a Text User Interface (TUI) application that allows you to browse, edit, and save Minecraft's NBT data structures directly from your terminal. NBT (Named Binary Tag) is the data format used by Minecraft to store various game data in .dat files.

## Features

- Browse through nested NBT structures with an intuitive interface
- View all tag types with their names and values
- Edit primitive values (byte, short, int, long, float, double, string)
- Add new tags to compound structures
- Delete existing tags
- Save changes back to the .dat file

## Requirements

- C++ compiler with C++11 support (g++, clang++)
- ncurses library for terminal UI
- Git (for cloning the repository)

## Installation

### Clone the repository

```bash
git clone https://github.com/yourusername/nbt-editor.git
cd nbt-editor
```

### Compile the project

```bash
g++ -o nbt_editor nbt_editor.cpp -lncurses
```

### Install (optional)

```bash
sudo cp nbt_editor /usr/local/bin/
```

## Usage

Run the editor by passing the NBT file as an argument:

```bash
./nbt_editor path/to/file.dat
```

Player data files can be found in the `playerdata` directory within each world save.

## Controls

| Key       | Function                            |
|-----------|-------------------------------------|
| ↑/↓       | Navigate through tags               |
| E         | Edit the value of the selected tag  |
| A         | Add a new tag to a compound         |
| D         | Delete the selected tag             |
| S         | Save changes to file                |
| Q         | Quit (prompts to save if modified)  |

## Supported NBT Tag Types

- TAG_End (0): Marks the end of compound tags
- TAG_Byte (1): Signed byte
- TAG_Short (2): Signed 16-bit integer
- TAG_Int (3): Signed 32-bit integer
- TAG_Long (4): Signed 64-bit integer
- TAG_Float (5): Single-precision floating point
- TAG_Double (6): Double-precision floating point
- TAG_Byte_Array (7): Array of bytes
- TAG_String (8): UTF-8 string
- TAG_List (9): List of nameless tags of the same type
- TAG_Compound (10): Collection of named tags
- TAG_Int_Array (11): Array of signed 32-bit integers
- TAG_Long_Array (12): Array of signed 64-bit integers

## Implementation Notes

This editor uses the ncurses library for terminal handling and implements the NBT binary format specification directly. The NBT format uses big-endian byte order for numeric values, which is properly handled in the reader/writer functions.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgements

- Minecraft NBT format specification
- ncurses library for terminal interfaces
