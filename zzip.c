#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "pq.h"
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 1024

typedef struct node {
	int freq;
	char ch;
	struct node *left;
	struct node *right;
} node;

int cmp(const void *d1, const void *d2)
{
	const node *n1 = (const node *)d1;
	const node *n2 = (const node *)d2;
	return n1->freq - n2->freq;
}

void free_tree(node *root)
{
	if (root == NULL)
		return;
	free_tree(root->left);
	free_tree(root->right);
	free(root);
}

/**
 * generate_codes - traverse huffman tree and generate codes
 */
void generate_codes(node *root, char *code, int depth, char codes[256][256])
{
	if (root->left == NULL && root->right == NULL) {
		code[depth] = '\0';
		strcpy(codes[(unsigned char)root->ch], code);

		return;
	}
	if (root->left != NULL) {
		code[depth] = '0';
		generate_codes(root->left, code, depth + 1, codes);
	}

	if (root->right != NULL) {
		code[depth] = '1';
		generate_codes(root->right, code, depth + 1, codes);
	}
}
/**
 * encode_file - outputs a frequency table and the compressed input file data to an ouput file
 */
void encode_file(FILE *in, char *filename, int *freq_tab, char codes[256][256],
		 size_t input_length)
{
	char *outname;
	asprintf(&outname, "%s.zzip", filename);
	FILE *out = fopen(outname, "wb");
	if (out == NULL) {
		perror("fopen");
		exit(1);
	}

	// Write the frequency table to binary
	if (fwrite(freq_tab, sizeof(int), 256, out) != 256) {
		perror("fwrite frequency table");
		exit(1);
	}

	char *buf = malloc(input_length);
	if (buf == NULL) {
		perror("malloc");
		exit(1);
	}

	rewind(in);
	if (fread(buf, sizeof(char), input_length, in) != input_length) {
		perror("fread");
		free(buf);
		exit(1);
	}

	unsigned char bit_buffer = 0;
	int bit_count = 0;

	for (size_t i = 0; i < input_length; i++) {
		char *code = codes[(unsigned char)buf[i]];
		for (int j = 0; code[j] != '\0'; j++) {
			if (code[j] == '1') {
				bit_buffer |= (1 << (7 - bit_count));
			}
			bit_count++;

			if (bit_count == 8) {
				if (fwrite(&bit_buffer, sizeof(unsigned char),
					   1, out) != 1) {
					perror("fwrite bit buffer");
					free(buf);
					exit(1);
				}
				bit_buffer = 0;
				bit_count = 0;
			}
		}
	}

	// Write remaining bits in bit_buffer
	if (bit_count > 0) {
		if (fwrite(&bit_buffer, sizeof(unsigned char), 1, out) != 1) {
			perror("fwrite remaining bits");
			free(buf);
			exit(1);
		}
	}

	free(buf);
	fclose(out);
	free(outname);
}

/**
 * read_freq_table - parse frequency table embedded in file
 */
void read_freq_table(FILE *in, int freq_tab[256])
{
	for (int i = 0; i < 256; i++) {
		fread(&freq_tab[i], sizeof(int), 1, in);
	}
}

/** 
 * generate_huffman_tree - builds a huffman tree and returns root
 *
 * @return root element of huffman tree 
 */
void *generate_huffman_tree(int *freq_tab)
{
	pq *pq = pq_init(cmp, 256);

	// add frequencies to priority queue
	for (int i = 0; i < 256; i++) {
		if (freq_tab[i] != 0) {
			node *n = malloc(sizeof *n);
			n->freq = freq_tab[i];
			n->ch = (char)i;
			n->left = n->right = NULL;
			pq_push(pq, n);
		}
	}

	// collapse into huffman tree
	while (pq->size > 1) {
		node *left = pq_pop(pq);
		node *right = pq_pop(pq);

		node *combo = malloc(sizeof(node));
		combo->freq = left->freq + right->freq;
		combo->left = left;
		combo->right = right;
		pq_push(pq, combo);
	}

	// return root
	return pq_pop(pq);
}

int main(int argc, char **argv)
{
	char codes[256][256] = { 0 };
	char code[256];

	int opt;
	bool decode_flag = false;

	while ((opt = getopt(argc, argv, "xo:")) != -1) {
		switch (opt) {
		case 'x':
			decode_flag = true;
			break;
		}
	}

	char *filename = argv[optind];
	FILE *in = fopen(filename, "r");
	if (in == NULL) {
		perror("fopen");
		return EXIT_FAILURE;
	}

	int freq_tab[256] = { 0 };
	char buffer[BUFFER_SIZE];

	size_t input_length = 0;
	node *root;

	if (decode_flag) {
		read_freq_table(in, freq_tab);
		root = generate_huffman_tree(freq_tab);
		unsigned char bit_buffer = 0;
		node *current = root;

		while (fread(&bit_buffer, sizeof(unsigned char), 1, in) == 1) {
			for (int i = 7; i >= 0; i--) {
				if (bit_buffer & (1 << i)) {
					current =
						current->right; // Bit is 1, go right
				} else {
					current =
						current->left; // Bit is 0, go left
				}

				// If we reach a leaf node, we've decoded a character
				if (current->left == NULL &&
				    current->right == NULL) {
					printf("%c", current->ch);
					current = root;
				}
			}
		}
	} else {
		size_t bytes_read;
		while ((bytes_read = fread(buffer, sizeof(char), BUFFER_SIZE,
					   in)) > 0) {
			input_length += bytes_read;
			for (size_t i = 0; i < bytes_read; i++) {
				freq_tab[(unsigned char)buffer[i]]++;
			}
		}
		root = generate_huffman_tree(freq_tab);
		generate_codes(root, code, 0, codes);
		encode_file(in, filename, freq_tab, codes, input_length);
	}

	free_tree(root);
	fclose(in);
	return EXIT_SUCCESS;
}
