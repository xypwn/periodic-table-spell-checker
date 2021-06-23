#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

const char* elements[] = {
	"__ROOT__", /* Not actually an element; represents the root node */
	"H",
	"He",
	"Li",
	"Be",
	"B",
	"C",
	"N",
	"O",
	"F",
	"Ne",
	"Na",
	"Mg",
	"Al",
	"Si",
	"P",
	"S",
	"Cl",
	"Ar",
	"K",
	"Ca",
	"Sc",
	"Ti",
	"V",
	"Cr",
	"Mn",
	"Fe",
	"Co",
	"Ni",
	"Cu",
	"Zn",
	"Ga",
	"Ge",
	"As",
	"Se",
	"Br",
	"Kr",
	"Rb",
	"Sr",
	"Y",
	"Zr",
	"Nb",
	"Mo",
	"Tc",
	"Ru",
	"Rh",
	"Pd",
	"Ag",
	"Cd",
	"In",
	"Sn",
	"Sb",
	"Te",
	"I",
	"Xe",
	"Cs",
	"Ba",
	"La",
	"Ce",
	"Pr",
	"Nd",
	"Pm",
	"Sm",
	"Eu",
	"Gd",
	"Tb",
	"Dy",
	"Ho",
	"Er",
	"Tm",
	"Yb",
	"Lu",
	"Hf",
	"Ta",
	"W",
	"Re",
	"Os",
	"Ir",
	"Pt",
	"Au",
	"Hg",
	"Tl",
	"Pb",
	"Bi",
	"Po",
	"At",
	"Rn",
	"Fr",
	"Ra",
	"Ac",
	"Th",
	"Pa",
	"U",
	"Np",
	"Pu",
	"Am",
	"Cm",
	"Bk",
	"Cf",
	"Es",
	"Fm",
	"Md",
	"No",
	"Lr",
	"Rf",
	"Db",
	"Sg",
	"Bh",
	"Hs",
	"Mt",
	"Ds",
	"Rg",
	"Cn",
	"Nh",
	"Fl",
	"Mc",
	"Lv",
	"Ts",
	"Og",
	NULL
};

/* Holds the index of a chem. element in `elements` */
typedef struct Node Node;
struct Node {
	ssize_t data;
	Node* children[2];
	Node* parent;
};
void Node_init(Node* obj) {
	obj->data = -1;
	obj->children[0] = NULL;
	obj->children[1] = NULL;
	obj->parent = NULL;
}
void Node_recursive_term(Node* obj) {
	size_t i;
	for(i = 0; i < 2; i++) {
		if(obj->children[i])
			Node_recursive_term(obj->children[i]);
	}
	free(obj);
}

/* Returns true, if both strings up to `len0` or `len1` are equal, disregarding the '\0' character, as the length must be given */
bool streq_len_ignore_case(const char* str0, size_t len0, const char* str1, size_t len1) {
	if(len0 != len1)
		return false;
	size_t i;
	for(i = 0; i < len0; i++) {
		char c0 = tolower(str0[i]);
		char c1 = tolower(str1[i]);
		if(c0 == '\0')
			break;
		if(c0 != c1)
			return false;
	}
	return true;
}

/* See if the input element string exists (ignoring case) */
ssize_t elms_contain(const char* str, size_t len) {
	size_t i;
	for(i = 0; elements[i] != NULL; i++) {
		if(streq_len_ignore_case(str, len, elements[i], strlen(elements[i])))
			return i;
	}
	return -1;
}

/* Prints all symbols in the path leading from the root node to the given leaf */
void print_path(Node* leaf) {
	size_t indices[512]; /* Maximum of 512 element symbols */
	size_t i;
	Node* curr;
	curr = leaf;
	i = 0;
	while(curr != NULL) {
		indices[i++] = curr->data;
		if(i >= 512) {
			/* If the word has more than 512 element symbols, throw an error */
			fprintf(stderr, "Word too large\n");
			exit(EXIT_FAILURE);
		}
		curr = curr->parent;
	}
	/* Print in reverse order */
	i--; /* Skip root node */
	while(i--)
		printf("%s ", elements[indices[i]]);
	printf("\n");
}

/* Returns true if it has reached the end of the string */
bool check_word_recursive(const char* in, Node* node) {
	bool ret = false; /* Return value; holds whether this branch has a solution for writing the desired word with element symbols */
	size_t i;
	/* 2 meaning to test for a max. element name length of 2 characters,
	 * where `i + 1` is the current element name length to test for */
	for(i = 0; i < 2; i++) {
		/* Don't continue looping, if the end of the input was reached */
		if(in[i] != '\0') {
			ssize_t el; /* (Chem.) element name index */
			el = elms_contain(in, i + 1); /* Test for an element name length of `i + 1`  */
			/* Don't do anything, if the result was -1 meaning no matching element was found (with first `i + 1` chars of `in` as a symbol) */
			if(el != -1) {
				Node* new;
				new = malloc(sizeof(Node));
				Node_init(new);
				new->data = el;
				new->parent = node;
				node->children[i] = new;
				if(in[i + 1] == '\0') {
					/* End of word was successfully reached using only element symbols */
					ret = true;
					print_path(new);
				} else {
					if(!check_word_recursive(in + i + 1, new)) {
						/* If the branch just created doesn't lead to the end of a word, destroy it */
						Node_recursive_term(new);
						node->children[i] = NULL;
					} else
						/* One of the children/subchildren has reportedly reached the end */
						ret = true;
				}
			}
		} else
			break;
	}
	return ret;
}

bool check_word(const char* input) {
	bool res;
	Node* root;
	root = malloc(sizeof(Node));
	Node_init(root);
	root->data = 0; /* 0 is root node idx(see `elements`) */

	res = check_word_recursive(input, root);

	Node_recursive_term(root);
	return res;
}

int main(int argc, const char** argv) {
	char buf[1024];
	bool res;
	printf("Enter a word to check for element symbol combinations:\n");
	scanf("%s", buf);
	res = check_word(buf);
	if(res) return 0;
	else return 1;
}
