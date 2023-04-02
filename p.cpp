#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <iostream>
#include <vector>
#include <map>
#include "disjoint_set.hpp" 
#include "disjoint_set.cpp"
#include <algorithm>

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

// So, this is for the scores...

// Pass everything by reference, to make it faster..... !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
map<int, vector<int>, greater<int> > analyze_board(Superball *&s, Disjoint_Set *&d) {
  int points_per_set, total_points = 0; 
  map<int, vector<int>, greater<int> > highest_scoring_set; 
    // printf("Scoring sets:");
    list <int>::const_iterator eit;
    int root;

    // LATEEEER.... I can try to optimize this to run only on the scoring cells... !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    for (int i = 0; i < d->Get_Set_Ids()->size(); i++) {
        root = d->Get_Set_Ids()->at(i); 
        if (d->Get_Elements()->at(root).size() >= s->mss) {
            for (eit = d->Get_Elements()->at(root).begin(); eit != d->Get_Elements()->at(root).end(); eit++) {
                //if (((*eit/s->c >= 2) && (*eit/s->c < (s->r - 2))) && ((*eit%s->c < 2) || (*eit%s->c >= (s->c - 2)))) {
                  if (s->goals[*eit]) {
                    cout << endl;
                    if (isalpha((char)s->board[*eit])) {
                      vector <int> scoring_set_cel; 
                      // printf("%2sSize:%3d%2sChar:%2c%2sScoring Cell:%3d,%d", "", (int)d->Get_Elements()->at(root).size(),"",(char)s->board[*eit],"",*eit/s->c,*eit%s->c);
                      cout << "(char)s->board[*eit] " << (char)s->board[*eit] << endl; 
                      cout << "size " << ((int)d->Get_Elements()->at(root).size()) << endl; 
                      switch ((char)s->board[*eit]) { // I think it's not necessary to typecast here... it would do it automatically when evaluation the case.. I think.. 
                        case 'P':
                        case 'p':
                          points_per_set = 2; 
                          break;
                        case 'B':
                        case 'b':
                          points_per_set = 3;
                          break;
                        case 'Y':
                        case 'y': 
                          points_per_set = 4; 
                          break;
                        case 'R':
                        case 'r':
                          points_per_set = 5;
                          break;
                        case 'G':
                        case 'g': 
                          points_per_set = 6;  
                          break;   
                      }
                      points_per_set *= ((int)d->Get_Elements()->at(root).size());
                      cout << "SCORE set " << points_per_set << endl;
                      scoring_set_cel.push_back(*eit/s->c);
                      scoring_set_cel.push_back(*eit%s->c);
                      highest_scoring_set.insert(make_pair(points_per_set, scoring_set_cel));
                      // total_points += (points_per_set)*((int)d->Get_Elements()->at(root).size()); 
                      break; // This is if I want to score the whole thing.. But I want to score particular groups.. 
                    }
                }
            }
        }
    }
    // cout << "Total points " << total_points << endl; 
    printf("\n");

    map<int, vector<int>, greater<int> >::iterator it_map; 
    for (it_map = highest_scoring_set.begin(); it_map != highest_scoring_set.end(); it_map++) {
      cout << "score " << it_map->first << " row / col "; 
      for (int i = 0; i < it_map->second.size(); i++) {
        cout << it_map->second.at(i) << " "; 
      }
      cout << endl;
    }

    return  highest_scoring_set;
}

void analyze_pairs(Superball *&s, Disjoint_Set *&d, int &num1, int &num2, multimap<int, int, greater<int> > &djs_sizes_descending_order) {
  multimap<int, int, greater<int> >::iterator it; 
  vector <int> pair; 
  pair.push_back(num1);
  pair.push_back(num2);
 



  /*
    // In the analyze function 
    For each pair: 
      -> Check whether both characters are alpha.. 
      -> 
  
  */
  //  if 

    // multimap<int, int, greater<int> >::iterator it; 

    // for (it = djs_sizes_descending_order.begin(); it != djs_sizes_descending_order.end(); it++) {
    //   cout << "it->first " << it->first << "    it->second " << it->second << endl; 
    // }

}

void enumerate_pairs(Superball *&s, Disjoint_Set *&d) {
  
    map<int, vector <int> > best_scores; // the minimum are the best... the closest.. 
    // I need to pass this to the analyze function, and insert as I go...
    // Ennumerate pair
    int start = 0; 
    for (int i = start; i < s->r*s->c; i++) { // replace with... s->r*s->c
        for (int j = (start+1); j < s->r*s->c; j++) {
            cout << i << " " << j << ", ";
            if (isalpha((char)s->board[i]) && isalpha((char)s->board[j]) && ((char)s->board[i] != (char)s->board[j])) {
              // analyze_pairs(s, d, djs_sizes_descending_order); 
            }
            // pass i and j to the analyze function.. 
            // So, I can first pass i j.. and then j i???????????????? 
        }
        cout << endl; 
        start++;
    }

    multimap<int, int, greater<int> > djs_sizes_descending_order; // size, index, descending order... 
    multimap<int, int, greater<int> >::iterator it; 

    for (int i = 0; i < d->Get_Sizes()->size(); i++) {
        //int root = d->Get_Set_Ids()->at(i); 
        djs_sizes_descending_order.insert(make_pair(d->Get_Sizes()->at(i), i));
    }

    // analyze_pairs(s, d, djs_sizes_descending_order); 
    // for (it = djs_sizes_descending_order.begin(); it != djs_sizes_descending_order.end(); it++) {
    //   cout << "it->first " << it->first << "    it->second " << it->second << endl; 
    // }

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
                // So, apparently I dont need to check this... why???????????????????????????????????????????????????????????
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

    cout << "\n\n";
    d->Print();
    cout << "\n\n";

  enumerate_pairs(s, d);
  //  for (int i = 0; i < d->Get_Set_Ids()->size(); i++) {
  //       int root = d->Get_Set_Ids()->at(i); 
  //       cout << "root " << root << endl;   
  //       cout << "size of set " << d->Get_Sizes()->at(root) << endl; 
  //  }

  // multimap<int, int, greater<int> > djs_sizes_descending_order; // size, index, descending order... 
  // multimap<int, int, greater<int> >::iterator it; 

  // vector <int> a; 
  // a.push_back(2); 
  // a.push_back(4); 
  // a.push_back(1);
  // a.push_back(0);

  // sort(a.begin(), a.end(), greater <>());
  //  for (int i = 0; i < a.size(); i++) {
  //       //int root = d->Get_Set_Ids()->at(i); 
  //       cout << "i " << i << " // ";   
  //       cout << "size of set " << a.at(i) << endl; 
  //   }

  //   cout << "*std::max_element(a.begin(), a.end())  " << *(a.begin()) << endl; 
  //   cout << "*(d->Get_Sizes()->begin())  " << *std::max_element(d->Get_Sizes()->begin(), d->Get_Sizes()->end()) << endl; 

    // Anyway .. if I order it, I'll lose the index...and I dont want that.. 
    // sort(d->Get_Sizes()->begin(), d->Get_Sizes()->end(), greater <>());

    // for (int i = 0; i < d->Get_Sizes()->size(); i++) {
    //     //int root = d->Get_Set_Ids()->at(i); 
    //     djs_sizes_descending_order.insert(make_pair(d->Get_Sizes()->at(i), i));
    // }

    // for (it = djs_sizes_descending_order.begin(); it != djs_sizes_descending_order.end(); it++) {
    //   cout << "it->first " << it->first << "    it->second " << it->second << endl; 
    // }


    // *std::max_element(scores.begin(), scores.end() )

    //  for (eit = d->Get_Elements()->at(root).begin(); eit != d->Get_Elements()->at(root).end(); eit++) {
    //             //if (((*eit/s->c >= 2) && (*eit/s->c < (s->r - 2))) && ((*eit%s->c < 2) || (*eit%s->c >= (s->c - 2)))) {
    //               if (s->goals[*eit]) {
    //                 cout << endl;
    //                 if (isalpha((char)s->board[*eit])) {

    // analyze_board(s, d); 

    // map<int, vector<int>, greater<int> > highest_scoring_set = analyze_board(s, d); 
    // map<int, vector<int>, greater<int> >::iterator it_map; 

    // for (it_map = highest_scoring_set.begin(); it_map != highest_scoring_set.end(); it_map++) {
    //   cout << "score " << it_map->first << " row / col "; 
    //   for (int i = 0; i < it_map->second.size(); i++) {
    //     cout << it_map->second.at(i) << " "; 
    //   }
    //   cout << endl;
    // }

// I need to grab the temporary file created by sb-player


// Okay, so I just need to print this.... 
// cout << "SWAP " << 3 << " " << 3 << " " << 2 << " " << 2;

// cout << "SCORE " << 3 << " " << 0; 

/*
yyggyryybp
ggrgpyppyp
RBgggyrpPP
GGgggybpPP
RGygryrpBP
YGyygyypYB
yyybpbyppb
ppggyypbbb
*/

}
