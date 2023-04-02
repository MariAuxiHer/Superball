/******************************************************************************************************************
    Maria Hernandez 
    COSC302
    03/10/2023
    Project3
    analyze.cpp
    This program analyzes a table given in an input file, finds all possible disjoint sets in that table, and 
	prints out a summary of the scoring sets.
*******************************************************************************************************************/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <iostream>
#include <vector>
#include "disjoint_set.hpp" 

using namespace std;
// using namespace plank;
using plank::Disjoint_Set;

#define talloc(type, num) (type *) malloc(sizeof(type)*(num))

// Create the Superball class.
class Superball {
  public:
    Superball(int argc, char **argv);
    int r;
    int c;
    int mss;
    int empty;
    vector <int> board;
    vector <int> goals;
    vector <int> colors;
};

void usage(const char *s) 
{
	fprintf(stderr, "usage: sb-play rows cols min-score-size colors\n");
  	if (s != NULL) fprintf(stderr, "%s\n", s);
  	exit(1);
}

// Initialize all fields of the Superball class.
Superball::Superball(int argc, char **argv)
{
  	int i, j;
  	string s;

  	if (argc != 5) usage(NULL);

	// Error checking input file.
  	if (sscanf(argv[1], "%d", &r) == 0 || r <= 0) usage("Bad rows");
  	if (sscanf(argv[2], "%d", &c) == 0 || c <= 0) usage("Bad cols");
  	if (sscanf(argv[3], "%d", &mss) == 0 || mss <= 0) usage("Bad min-score-size");

	// Setting up the colors vector. The indexes having the decimal representation of the characters pbyrg and PBYRG
	// will contain the value of each of these. 
  	colors.resize(256, 0);

  	for (i = 0; i < (int)strlen(argv[4]); i++) {
    	if (!isalpha(argv[4][i])) usage("Colors must be distinct letters");
    	if (!islower(argv[4][i])) usage("Colors must be lowercase letters");
    	if (colors[argv[4][i]] != 0) usage("Duplicate color");
    	colors[argv[4][i]] = 2+i;
    	colors[toupper(argv[4][i])] = 2+i;
  	}

  	board.resize(r*c);
  	goals.resize(r*c, 0);

  	empty = 0;

	// Setting up the board.
  	for (i = 0; i < r; i++) {
    	if (!(cin >> s)) {
      		fprintf(stderr, "Bad board: not enough rows on standard input\n");
      		exit(1);
    	}
    	if ((int)s.size() != c) {
      		fprintf(stderr, "Bad board on row %d - wrong number of characters.\n", i);
      		exit(1);
    	}
    	for (j = 0; j < c; j++) {
      		if (s[j] != '*' && s[j] != '.' && colors[s[j]] == 0) {
        		fprintf(stderr, "Bad board row %d - bad character %c.\n", i, s[j]);
        		exit(1);
      		}
      		board[i*c+j] = s[j];
      		if (board[i*c+j] == '.') empty++;
      		if (board[i*c+j] == '*') empty++;
      		if (isupper(board[i*c+j]) || board[i*c+j] == '*') {
        		goals[i*c+j] = 1;
        		board[i*c+j] = tolower(board[i*c+j]);
      		}
    	}
  	}
}

// Function that prints a summary of the scoring sets.
void print_scoring_sets(Superball *&s, Disjoint_Set *&d) {
    printf("Scoring sets:");
    list <int>::const_iterator eit;
    int root;

	// Loop through the vector returned by Get_Set_Ids(), which contains all the root nodes. 
    for (size_t i = 0; i < d->Get_Set_Ids()->size(); i++) {
        root = d->Get_Set_Ids()->at(i); 

		// If the size set is greater than the minimum scoring size, we loop through the set and check whether it contains an element present 
		// in the scoring cells. We then output a summary of that set.
        if ((int)d->Get_Elements()->at(root).size() >= s->mss) {
            for (eit = d->Get_Elements()->at(root).begin(); eit != d->Get_Elements()->at(root).end(); eit++) {
                if (s->goals[*eit]) {
                    cout << endl;
                    if (isalpha((char)s->board[*eit])) {
                      	printf("%2sSize:%3d%2sChar:%2c%2sScoring Cell:%3d,%d", "", (int)d->Get_Elements()->at(root).size(),"",(char)s->board[*eit],"",*eit/s->c,*eit%s->c);
                      	break;
                    }
                }
            }
        }
    }
    printf("\n");
}

int main(int argc, char **argv)
{
    Superball *s;
 
    s = new Superball(argc, argv);
    int s1, s2;

    Disjoint_Set *d = new Disjoint_Set();; 
    d->Initialize(s->r*s->c);

	// Analyze the board.
	// The following nested loops allow us to handle the 1D vector as a 2D vector where the outer loop loops through the rows and 
	// the inner loop loops through the columns. 
    for (int i = 0; i < s->r; i++) {
        for (int j = 0; j < s->c; j++) {

			// If the character at the given cell is alpha, we examine the cells next to it and below to see whether they contain 
			// the same alpha character. 
            if (isalpha((char)s->board[i*s->c+j])) {
                if (j < (s->c-1)) {

					// If the cell next to the current contain the same element, we find the root node of both, and if they have different
					// roots, we join them. 
                    if (tolower((char)s->board[i*s->c+(j+1)]) == tolower((char)s->board[i*s->c+j])) {
                        s1 = (d->Find(i*s->c+(j)));
                        s2 = (d->Find(i*s->c+(j+1)));
                        if (s1 != s2)  d->Union(s1, s2);
                    }
                }

                if (i < s->r-1) {

					// If the cell below the current contain the same element, we find the root node of both, and if they have different
					// roots, we join them. 
                    if (tolower((char)s->board[(i+1)*s->c + j]) == tolower((char)s->board[i*s->c+j])) {
                        s1 = (d->Find(i*s->c+(j)));
                        s2 = (d->Find((i+1)*s->c + j));
                        if (s1 != s2)  d->Union(s1, s2);
                    }
                }
            }
        }
    }

    print_scoring_sets(s, d);
}