# zzip: A Huffman Coding Compression Utility

`zzip` is a command-line utility that uses Huffman coding to compress and decompress files. This tool efficiently reduces file sizes by encoding the most frequent characters with shorter bit sequences, while less frequent characters are represented with longer bit sequences.

## Features

- **File Compression**: Compress files using Huffman coding, reducing the file size while preserving all original data.
- **File Decompression**: Decompress previously compressed `.zzip` files and output the result to `stdout`.
- **Efficient Memory Usage**: The implementation uses a binary heap-based priority queue to manage the construction of the Huffman tree.

## How It Works

### Compression

1. **Frequency Analysis**: The program first reads the input file and calculates the frequency of each character.
2. **Huffman Tree Construction**: Using a priority queue (implemented as a binary heap), the program builds a Huffman tree, where each node represents a character and its frequency.
3. **Code Generation**: The tree is traversed to assign a binary code to each character, with shorter codes assigned to more frequent characters.
4. **Encoding**: The input file is then encoded using the generated Huffman codes, and the compressed data is written to a new file with a `.zzip` extension.

### Decompression

1. **Tree Reconstruction**: The decompression process reconstructs the Huffman tree from the compressed file's metadata.
2. **Decoding**: The encoded data is decoded by traversing the Huffman tree according to the bit sequences in the compressed file, reproducing the original file's content.
3. **Output**: The decoded content is output directly to `stdout`.

## Command-Line Usage

### Compression

To compress a file, run the following command:

```bash
./zzip filename
```

This will create a compressed file named filename.zzip in the current directory.

### Decompression
To decompress a file and output the content to stdout, use the -x flag:

```bash
./zzip -x filename.zzip > filename
```
## Implementation Details
- **Priority Queue** (pq.c/pq.h): The priority queue is used to facilitate the construction of the Huffman tree. It is implemented as a binary heap, allowing efficient insertion and removal of elements.
- **Huffman Coding**: The core of the compression relies on Huffman coding, a well-known algorithm for lossless data compression. The implementation ensures that the most frequent characters are represented with the fewest bits.
- **Memory Management**: Dynamic memory allocation is used for managing the Huffman tree and the priority queue, ensuring efficient use of system resources.

## Areas for Improvement
- **Error Handling**: The current implementation could be improved by introducing more robust error handling, particularly around file I/O operations and memory allocation failures.
- **File Format**: While the .zzip format is functional, it could be enhanced by adding a standardized file header that stores metadata such as the original file size and compression ratio.
- **Performance Optimizations**: Additional optimizations could be applied to further reduce the compression and decompression times, especially for very large files.
- **Extended Functionality**: Support for compressing multiple files in a single archive or handling different types of input data (e.g., binary files) could be added to broaden the utility of the program.
- **Testing and Validation**: Unit tests and performance benchmarks would help ensure the reliability and efficiency of the program under various conditions.
