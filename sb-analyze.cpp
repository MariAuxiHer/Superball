#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <iostream>
#include <vector>
//#include "disjoint.h"
//#include "disjoint-rank.cpp"
#include "disjoint_set.hpp" 
//#include "disjoint_set.cpp"

// or without ./
using namespace std;
using namespace plank;

#define talloc(type, num) (type *) malloc(sizeof(type)*(num))


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

Superball::Superball(int argc, char **argv)
{
  int i, j;
  string s;

  if (argc != 5) usage(NULL);

  if (sscanf(argv[1], "%d", &r) == 0 || r <= 0) usage("Bad rows");
  if (sscanf(argv[2], "%d", &c) == 0 || c <= 0) usage("Bad cols");
  if (sscanf(argv[3], "%d", &mss) == 0 || mss <= 0) usage("Bad min-score-size");

  colors.resize(256, 0);

  for (i = 0; i < strlen(argv[4]); i++) {
    if (!isalpha(argv[4][i])) usage("Colors must be distinct letters");
    if (!islower(argv[4][i])) usage("Colors must be lowercase letters");
    if (colors[argv[4][i]] != 0) usage("Duplicate color");
    colors[argv[4][i]] = 2+i;
    colors[toupper(argv[4][i])] = 2+i;
  }

  board.resize(r*c);
  goals.resize(r*c, 0);

  empty = 0;

  for (i = 0; i < r; i++) {
    if (!(cin >> s)) {
      fprintf(stderr, "Bad board: not enough rows on standard input\n");
      exit(1);
    }
    if (s.size() != c) {
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

void analyze_board(Superball *&s, Disjoint_Set *&d) {
    printf("Scoring sets:");
    list <int>::const_iterator eit;
    int root;
    for (int i = 0; i < d->Get_Set_Ids()->size(); i++) {
        root = d->Get_Set_Ids()->at(i); 
        // if (d->Get_Elements()->at(root).empty()) {
        //     printf("root = %d\n", root);
        //     throw std::logic_error("Disjoint_Set::Print() - empty=0");
        // }

        if (d->Get_Elements()->at(root).size() >= s->mss) {
            for (eit = d->Get_Elements()->at(root).begin(); eit != d->Get_Elements()->at(root).end(); eit++) {
                //if (((*eit/s->c >= 2) && (*eit/s->c < (s->r - 2))) && ((*eit%s->c < 2) || (*eit%s->c >= (s->c - 2)))) {
                  if (s->goals[*eit/*((*eit/s->c)*(s->c)) + (*eit%s->c)*/] == 1) {
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

    for (int i = 0; i < s->r; i++) {
        for (int j = 0; j < s->c; j++) {
            if (isalpha((char)s->board[i*s->c+j])) {
                if (j < (s->c-1)) {
                    if (tolower((char)s->board[i*s->c+(j+1)]) == tolower((char)s->board[i*s->c+j])) {
                        s1 = (d->Find(i*s->c+(j)));
                        s2 = (d->Find(i*s->c+(j+1)));
                        if (s1 != s2)  d->Union(s1, s2);
                    }
                }
                if (i < s->r-1) {
                    if (tolower((char)s->board[(i+1)*s->c + j]) == tolower((char)s->board[i*s->c+j])) {
                        s1 = (d->Find(i*s->c+(j)));
                        s2 = (d->Find((i+1)*s->c + j));
                        if (s1 != s2)  d->Union(s1, s2);
                    }
                }
            }
        }
    }

    d->Print();
    analyze_board(s, d);

    // printf("Scoring sets:");
    // list <int>::const_iterator eit;
    // int root;
    // for (int i = 0; i < d->Get_Set_Ids()->size(); i++) {
    //     root = d->Get_Set_Ids()->at(i); 
    //     if (d->Get_Elements()->at(root).empty()) {
    //         printf("root = %d\n", root);
    //         throw std::logic_error("Disjoint_Set::Print() - empty=0");
    //     }

    //     if (d->Get_Elements()->at(root).size() >= s->mss) {
    //         for (eit = d->Get_Elements()->at(root).begin(); eit != d->Get_Elements()->at(root).end(); eit++) {
    //             //if (((*eit/s->c >= 2) && (*eit/s->c < (s->r - 2))) && ((*eit%s->c < 2) || (*eit%s->c >= (s->c - 2)))) {
    //               if (s->goals[((*eit/s->c)*(s->c)) + (*eit%s->c)] == 1) {
    //                 cout << endl;
    //                 if (isalpha((char)s->board[*eit])) {
    //                   printf("%2sSize:%3d%2sChar:%2c%2sScoring Cell:%3d,%d", "", (int)d->Get_Elements()->at(root).size(),"",(char)s->board[*eit],"",*eit/s->c,*eit%s->c);
    //                   break;
    //                 }
    //             }
    //         }
    //     }
    // }
    // printf("\n");
}
