# BWT Encoder/Decoder

This project provides two command line programs for Burrows-Wheeler transform (BWT) encoding and decoding of text files:
- `bwtsearch`: searches for a given pattern in the BWT encoded file
- `bwtdecode`: decodes a BWT encoded file back to its original format

## Getting Started

### Prerequisites

The project requires the following software to be installed on the system:
- GCC (GNU Compiler Collection) for compiling the C code

### Building the Programs

To build the programs, navigate to the project directory and run the following command:

make


This will compile both the `bwtsearch` and `bwtdecode` programs and create the executable files in the same directory.

### Running the Programs

#### `bwtsearch`

To search for a pattern in a BWT encoded file, run the following command:

./bwtsearch <BWT file path> <pattern>


This will output the positions of the pattern in the original file.

#### `bwtdecode`

To decode a BWT encoded file, run the following command:

./bwtdecode <BWT file path> <reversed file path>

