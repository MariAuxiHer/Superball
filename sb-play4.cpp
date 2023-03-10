// so far this one has the best version........ 767.900 for seed 1 

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
#include <list>

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

void create_sets (Superball *&s, Disjoint_Set &d) {
    d.Initialize(s->r*s->c);
    int s1, s2;
    for (int i = 0; i < s->r; i++) {
        for (int j = 0; j < s->c; j++) {
            if (isalpha((char)s->board[i*s->c+j])) {
                if (j < (s->c-1)) {
                    if (s->board[i*s->c+(j+1)] == s->board[i*s->c+j]) {
                        s1 = (d.Find(i*s->c+(j)));
                        s2 = (d.Find(i*s->c+(j+1)));
                        if (s1 != s2)  d.Union(s1, s2);
                    }
                }
                // So, apparently I dont need to check this... why???????????????????????????????????????????????????????????
                if (i < s->r-1) {
                    if (s->board[(i+1)*s->c + j] == s->board[i*s->c+j]) {
                        s1 = (d.Find(i*s->c+(j)));
                        s2 = (d.Find((i+1)*s->c + j));
                        if (s1 != s2)  d.Union(s1, s2);
                    }
                }
            }
        }
    }
}

// Pass everything by reference, to make it faster..... !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

void print_board(Superball *&s) {
    for (int i = 0; i < s->board.size(); i++) {
    //cout << "i " << i << " "; 
    cout << (char)s->board.at(i) << " ";
    if ((i+1)%10 == 0) cout << endl; 
  }
}

void score_each_swap(Superball *&s, Disjoint_Set &d, int &num1, int &num2, multimap<int, vector<int>, greater<int> > &all_swap_scoring, int greatest_set_in_scoring_cell, int total_size_in_scoring_cell) {
  int points_per_set, total_points = 0; 
  // multimap<int, vector<int>, greater<int> > all_swap_scoring; 
   vector <int> swap_pair; 
    // printf("Scoring sets:");
    list <int>::const_iterator eit;
    int root;
    //cout << "greatest_set_in_scoring_cell in SWAAAAAPS " << greatest_set_in_scoring_cell << endl;
// print_board(s);
///////////////
    multimap<int, int, greater<int> > djs_sizes_descending_order; // size, index, descending order... 
    multimap<int, int, greater<int> >::iterator djs_sizes_descending_order_it; 
    int total_size_after_swap = 0; 
    int greatest_set_after_swap; 
  
  //cout << "heeeeey iuuuuuuuuu" << endl;
        for (int i = 0; i < d.Get_Set_Ids()->size(); i++) {
        root = d.Get_Set_Ids()->at(i); 
        // if (d.Get_Elements()->at(root).empty()) {
        //     printf("root = %d\n", root);
        //     throw std::logic_error("Disjoint_Set::Print() - empty=0");
        // }
            for (eit = d.Get_Elements()->at(root).begin(); eit != d.Get_Elements()->at(root).end(); eit++) {
                //if (((*eit/s->c >= 2) && (*eit/s->c < (s->r - 2))) && ((*eit%s->c < 2) || (*eit%s->c >= (s->c - 2)))) {
                  if (s->goals[*eit/*((*eit/s->c)*(s->c)) + (*eit%s->c)*/] == 1) {
                    if (isalpha((char)s->board[*eit])) {
                      // printf("%2sSize:%3d%2sChar:%2c%2sScoring Cell:%3d,%d", "", (int)d.Get_Elements()->at(root).size(),"",(char)s->board[*eit],"",*eit/s->c,*eit%s->c);
                      int size = d.Get_Sizes()->at(root);
                      djs_sizes_descending_order.insert(make_pair(size, root));
                      break;
                    }
                }
            }
        }
    for (djs_sizes_descending_order_it = djs_sizes_descending_order.begin(); djs_sizes_descending_order_it != djs_sizes_descending_order.end(); djs_sizes_descending_order_it++) {
        //cout << djs_sizes_descending_order_it->first << " " << djs_sizes_descending_order_it->second << endl; 
        total_size_after_swap+= djs_sizes_descending_order_it->first;
    }

    djs_sizes_descending_order_it = djs_sizes_descending_order.begin();
    greatest_set_after_swap  = djs_sizes_descending_order_it->first;

// if (num1 == 32 && num2 == 49) {
//     for (djs_sizes_descending_order_it = djs_sizes_descending_order.begin(); djs_sizes_descending_order_it != djs_sizes_descending_order.end(); djs_sizes_descending_order_it++) {
//          cout << djs_sizes_descending_order_it->first << " " << djs_sizes_descending_order_it->second << endl; 
//     }
//       cout << "num1 " << num1 << "num2 " << num2 << endl; 
//     cout << "total_size_after_swap " << total_size_after_swap << endl;
//     cout << "greatest_set_after_swap  " << greatest_set_after_swap  << endl;
// }
    // djs_sizes_descending_order_it = djs_sizes_descending_order.begin();
    // int greatest_size = djs_sizes_descending_order_it->first;


    // try first the greatest sizes of sets in score cells... and then only the greatest size of the present one greater than the 
    // greatest of the original one.. 
    /////////////////////

/*
                      gg..b....p
                      ......r...
                      **p.....**
                      *Bbrp...**
                      **...pbbBP
                      **p...y.**
                      ....yy..r.
                      ..gg.yr.g.
                      */    


    // cout << "\n\n";
    // for (it = djs_sizes_descending_order.begin(); it != djs_sizes_descending_order.end(); it++) {
    //     cout << "it->first " << it->first << "    it->second " << it->second << endl; 
    // }

    //       print_board(s);
    // LATEEEER.... I can try to optimize this to run only on the scoring cells... !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    for (int i = 0; i < d.Get_Set_Ids()->size(); i++) {
      // I may be able to keep track of the sizes here as well.. 
      // so d.Get_Set_Ids() has all root nodes... and i can count the sets that are greater than one... 
      // the combination that produces the greatest size wins... although, this is later after trying only score.......
      // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        root = d.Get_Set_Ids()->at(i); 
        // it wouldn't make so much sense to compare the sets of 1....
        if (d.Get_Elements()->at(root).size() >= 2) {
            eit = d.Get_Elements()->at(root).begin();
            //for (eit = d.Get_Elements()->at(root).begin(); eit != d.Get_Elements()->at(root).end(); eit++) {
                //if (((*eit/s->c >= 2) && (*eit/s->c < (s->r - 2))) && ((*eit%s->c < 2) || (*eit%s->c >= (s->c - 2)))) {
                  //if (s->goals[*eit]) {
                   //    cout << endl;

                    //   cout << "START " << endl; 
                    //if (isalpha((char)s->board[*eit])) {
                      // printf("%2sSize:%3d%2sChar:%2c%2sScoring Cell:%3d,%d", "", (int)d.Get_Elements()->at(root).size(),"",(char)s->board[*eit],"",*eit/s->c,*eit%s->c);
                    //     cout << "CELL " << *eit << endl; 
                    
                    //    cout << "(char)s->board[*eit] " << (char)s->board[*eit] << endl; 

                    //     cout << "SWWAAAAAAAP PAIR " << num1 << " // " << num2 << endl;
                      // cout << "size " << ((int)d.Get_Elements()->at(root).size()) << endl; 
                      

                      /*
                      gg..b....p
                      ......r...
                      **p.....**
                      *Bbrp...**
                      **...pbbBP
                      **p...y.**
                      ....yy..r.
                      ..gg.yr.g.
                      */


                      /*
                      Look for a way to give it more weight to the biggest sizes in goal....!!!!!!!!!!!!!! very important...
                      Figure it out with this example...
                      */
                      switch ((char)s->board[*eit]) { // I think it's not necessary to typecast here... it would do it automatically when evaluation the case.. I think.. 
                        //case 'P':
                        case 'p':
                          points_per_set = 2; 
                          break;
                        //case 'B':
                        case 'b':
                          points_per_set = 3;
                          break;
                        //case 'Y':
                        case 'y': 
                          points_per_set = 4; 
                          break;
                        //case 'R':
                        case 'r':
                          points_per_set = 5;
                          break;
                        //case 'G':
                        case 'g': 
                          points_per_set = 6;  
                          break;   
                      }
                      // multiplying by the size of the set.. 
                      points_per_set *= ((int)d.Get_Elements()->at(root).size());

                      for (eit = d.Get_Elements()->at(root).begin(); eit != d.Get_Elements()->at(root).end(); eit++) {
                        // if this is in goal cell and this set is of the greatest size in scoring cell.. 
                        if (s->goals[*eit]) {
                        // if this set is of the greatest size in scoring cell.. multiply by 1000
                        //  cout << "d.Get_Sizes()->at(d.Find(*eit)) " << d.Get_Sizes()->at(d.Find(*eit)) << endl; 
                        // if (d.Get_Sizes()->at(d.Find(*eit)) == greatest_size) {
                        if (total_size_after_swap >= total_size_in_scoring_cell && greatest_set_after_swap > greatest_set_in_scoring_cell) {
                            points_per_set *= 1000;
                            break;
                          }
                          else {
                            points_per_set *= 100;
                            break;
                          }
                          // otherwise, multiply by 100...
                        }
                      }
                      //  cout << "SCORE set " << points_per_set << endl;    
            //   cout << "END " << endl; 
        total_points += points_per_set;
        }
    // printf("\n");
    
    //return  all_swap_scoring;
}
  //     cout << "Total points " << total_points << endl; 

        swap_pair.push_back(num1);
        swap_pair.push_back(num2);
        all_swap_scoring.insert(make_pair(total_points, swap_pair));

}

// If I use this for swap, I wouldn't score right away what I have... 
// This may work like for prediction and stuff..... 
multimap<int, vector<int>, greater<int> > is_there_something_to_score(Superball *&s, Disjoint_Set &d) {
  int points_per_set, total_points = 0; 
  multimap<int, vector<int>, greater<int> > highest_scoring_set; 
    // printf("Scoring sets:");
    list <int>::const_iterator eit;
    int root;

    // LATEEEER.... I can try to optimize this to run only on the scoring cells... !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    for (int i = 0; i < d.Get_Set_Ids()->size(); i++) {
      // I may be able to keep track of the sizes here as well.. 
      // so d.Get_Set_Ids() has all root nodes... and i can count the sets that are greater than one... 
      // the combination that produces the greatest size wins... although, this is later after trying only score.......
      // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        root = d.Get_Set_Ids()->at(i); 
        if (d.Get_Elements()->at(root).size() >= s->mss) {
            for (eit = d.Get_Elements()->at(root).begin(); eit != d.Get_Elements()->at(root).end(); eit++) {
                //if (((*eit/s->c >= 2) && (*eit/s->c < (s->r - 2))) && ((*eit%s->c < 2) || (*eit%s->c >= (s->c - 2)))) {
                  if (s->goals[*eit]) {
                    //cout << endl;

                    // cout << "START " << endl; 
                    if (isalpha((char)s->board[*eit])) {
                      vector <int> scoring_set_cel; 
                      // printf("%2sSize:%3d%2sChar:%2c%2sScoring Cell:%3d,%d", "", (int)d.Get_Elements()->at(root).size(),"",(char)s->board[*eit],"",*eit/s->c,*eit%s->c);
                      //  cout << "(char)s->board[*eit] " << (char)s->board[*eit] << endl; 
                      //cout << "size " << ((int)d.Get_Elements()->at(root).size()) << endl; 
                      switch ((char)s->board[*eit]) { // I think it's not necessary to typecast here... it would do it automatically when evaluation the case.. I think.. 
                        //case 'P':
                        case 'p':
                          points_per_set = 2; 
                          break;
                        //case 'B':
                        case 'b':
                          points_per_set = 3;
                          break;
                        //case 'Y':
                        case 'y': 
                          points_per_set = 4; 
                          break;
                        //case 'R':
                        case 'r':
                          points_per_set = 5;
                          break;
                        //case 'G':
                        case 'g': 
                          points_per_set = 6;  
                          break;   
                      }
                      // multiplying by the size of the set.. 
                      points_per_set *= ((int)d.Get_Elements()->at(root).size());
                      // cout << "SCORE set " << points_per_set << endl;
                      scoring_set_cel.push_back(*eit/s->c);
                      scoring_set_cel.push_back(*eit%s->c);
                      highest_scoring_set.insert(make_pair(points_per_set, scoring_set_cel));
                      // total_points += (points_per_set)*((int)d.Get_Elements()->at(root).size()); 
                      break; // This is if I want to score the whole thing.. But I want to score particular groups.. 
                    }
                }
            }
            // cout << "END " << endl; 
        }
    }
    //cout << "Total points " << total_points << endl; 
    //printf("\n");

    // multimap<int, vector<int>, greater<int> >::iterator it_multimap; 
    // for (it_multimap = highest_scoring_set.begin(); it_multimap != highest_scoring_set.end(); it_multimap++) {
    //   // cout << "score " << it_multimap->first << " row / col "; 
    //   for (int i = 0; i < it_multimap->second.size(); i++) {
    //     // cout << it_multimap->second.at(i) << " "; 
    //   }
    //   // cout << endl;
    // }

    return  highest_scoring_set;
}


void swap(Superball *&s, int &num1, int &num2) {
  int temp = s->board[num1];
  s->board[num1] = s->board[num2];
  s->board[num2] =  temp;
}


// first test the calculate_distance function by itself..... passing the values expected... CHEEEEEECKED...
// then the closest_distance...
// and then the enumerate_pairs_closest_distance.....
int calculate_distance (int &col, int &num1, list<int> set) {
    int min_element = *std::min_element(set.begin(), set.end());
    int max_element = *std::max_element(set.begin(), set.end()); 
    int distance;
    // cout << "min_element " << min_element << "max_element " << max_element << endl; 
    // cout << "NUMBER " << num1 << endl; 
    list<int>::iterator it; 

    /*  
    .r........
    .....rb...
    **g....b**
    **....b.**
    **.g....*Y
    **......*P
    .....r....
    ..b.......  



it->first 1    it->second 1
it->first 1    it->second 15
it->first 1    it->second 16
it->first 1    it->second 22
it->first 1    it->second 27
it->first 1    it->second 36
it->first 1    it->second 43
it->first 1    it->second 49
it->first 1    it->second 59
it->first 1    it->second 65
it->first 1    it->second 72
    */
    
    if (num1 < min_element) {
        // cout << "min " << endl; 
        if (num1%col <= min_element%col) {
            distance = abs(num1%col - min_element%col) + abs(num1/col - min_element/col);
        }
        else {
            // cout << "min but greater col " << endl; 
            distance = -1; 
            for (it = set.begin(); it != set.end(); it++) {
                // cout << "dist outside " << abs(num1%col - *it%col) + abs(num1/col - *it/col) << " current distance " << distance << endl; 
                if (distance == -1 || abs(num1%col - *it%col) + abs(num1/col - *it/col) < distance) {
                    distance = abs(num1%col - *it%col) + abs(num1/col - *it/col);
                    // cout << "dist " << distance << endl; 
                }
            }
        }
    }
    else if (num1 > max_element) {
        // cout << "max " << endl; 
        // if element diff to zero..... try later zero case......  
        if (num1%col >= max_element%col) {
            distance = abs(num1%col - max_element%col) + abs(num1/col - max_element/col);
        }
        else {
            // cout << "max but less col " << endl; 
            distance = -1; 
            for (it = set.begin(); it != set.end(); it++) {
                // cout << "dist outside " << abs(num1%col - *it%col) + abs(num1/col - *it/col) << " current distance " << distance << endl; 
                if (distance == -1 || abs(num1%col - *it%col) + abs(num1/col - *it/col) < distance) {
                    distance = abs(num1%col - *it%col) + abs(num1/col - *it/col);
                    // cout << "dist " << distance << endl; 
                }
            }
        }
    }
    else if (num1 > min_element && num1 < max_element) {
        // cout << "middle " << endl; 
        distance = -1; 
        for (it = set.begin(); it != set.end(); it++) {
            if (num1/col == *it/col) {
                // cout << "dist outside " << abs(num1%col - *it%col) << " current distance " << distance << endl; 
                if (distance == -1 || abs(num1%col - *it%col) < distance) {
                    distance = abs(num1%col - *it%col);
                    // cout << "dist " << distance << endl; 
                }
            }
        }
    }
    // cout << "distance to set " << distance << endl; 
    return distance;
}

void closest_distance(Superball *&s, Disjoint_Set &d, int &num1, int &num2, multimap<int, vector <int> > &scores_min_distances, multimap<int, vector <int> > &none_min_distance, multimap<int, int, greater<int> > &djs_sizes_descending_order) {
    multimap<int, int, greater<int> >::iterator it; 
    vector <int> swap_pair; 
    vector <int> pair_none_min_distance; 
    int none_min_distance_value = 0; 
    it = djs_sizes_descending_order.begin();
    int original_distance = 0; 
    int new_distance = 0;
    int greatest_current_sets = it->first; 
    list<int>::iterator find_num1; 
    list<int>::iterator find_num2; 
    list<int> set;

    // list<int> listOfStrs2 = listOfStrs;
    // // Check if an element exists in list
    // // Create a list Iterator
    // list<int>::iterator it;
    // // Fetch the iterator of element with value 'the'
    // it = std::find(listOfStrs2.begin(), listOfStrs2.end(), 4);

    // looping through only my biggest sets to find the smallest distance of a swap to one of these sets....
    // while (it->first == greatest_current_sets && (it != djs_sizes_descending_order.end())) {
    while (it != djs_sizes_descending_order.end()) {
        // calculating original distaces..... ofc.. so I don't wanna calculate the distance between the greatest set and the num if these 
        // are not equal... the idea is to put similar letters as close as possible... 

/*
..........
...rr.....
**......**
**....b.**
**.p....**
**......**
..........
..........
*/
        /*
        ..........
        ..g.......
        **g.....**
        **.p....**
        **.g....**
        **......**
        ..........
        ..........
        end()


        .r........
        ..........
        **g....b**
        **....b.**
        **.g....*Y
        **......*P
        .....rr...
        ..b.......
        */
       set = d.Get_Elements()->at(it->second);
       // it = std::find(listOfStrs.begin(), listOfStrs.end(), "the");
       find_num1 = find(set.begin(), set.end(), num1);
       find_num2 = find(set.begin(), set.end(), num2);

// && d.Get_Elements()->at(it->second).size() < 2

/*
Accounting for multiple sets of the same greatest size... 
No comparing distance with the set itself.. (num1 and num2 cant be part of the set)
No destroying sets with size greater or equal than 2..
just bring an element of size 1 closer together to a bigger set.. 


.r........
...rrr....
**g.....RR
**....b.**
**.g....*Y
**......*Y
.....rr...
..b.......
possible swaps 
1 22 min dist less
1 36 min dist now less


*/
        if ((find_num1 == set.end()) && (find_num2 == set.end()) && (d.Get_Elements()->at(d.Find(num1)).size() < 2) && (d.Get_Elements()->at(d.Find(num2)).size() < 2)) {


        //        cout << "num1 " << num1 << endl; 
        // cout << "num2 " << num2 << endl; 
        // cout << "it->second " << it->second << endl; 



        if (s->board[num1] == s->board[it->second]) {
            // calculate distance 
            original_distance = calculate_distance (s->c, num1, d.Get_Elements()->at(it->second)); // ???????????????????? d.Get_Elements()->at(it->second) this is the list, right????????????
            // cout << "In 1 " << original_distance << endl;
        }
        if (s->board[num2] == s->board[it->second]) {
            // calculate distance 
            original_distance = calculate_distance (s->c, num2, d.Get_Elements()->at(it->second)); 
            // cout << "In 2 " << original_distance << endl;
        }

        swap(s, num1, num2);

        if (s->board[num1] == s->board[it->second]) {
            // calculate distance 
            new_distance = calculate_distance (s->c, num1, d.Get_Elements()->at(it->second)); // ???????????????????? d.Get_Elements()->at(it->second) this is the list, right????????????
        //     cout << "In 3 " << new_distance << endl;
        //      cout << "NEWWWWW DISTANCEEE " << new_distance << endl;
        // cout << "ORIGINALL DISTANCEEE " << original_distance << endl;

            if (new_distance < original_distance) {
            // cout << "YES " << endl;  
            swap_pair.push_back(num1);
            swap_pair.push_back(num2);
           // swap_pair.push_back(it->second);
            scores_min_distances.insert(make_pair(new_distance, swap_pair));
            swap_pair.clear();
        }
       
        }

        if (s->board[num2] == s->board[it->second]) {
            // calculate distance 
            new_distance = calculate_distance (s->c, num2, d.Get_Elements()->at(it->second)); 
        //     cout << "In 4 " << new_distance << endl;

        //      cout << "NEWWWWW DISTANCEEE " << new_distance << endl;
        // cout << "ORIGINALL DISTANCEEE " << original_distance << endl;
            if (new_distance < original_distance) {
              // cout << "YES " << endl; 
            swap_pair.push_back(num1);
            swap_pair.push_back(num2);
           // swap_pair.push_back(it->second);
            scores_min_distances.insert(make_pair(new_distance, swap_pair));
            swap_pair.clear();
        }
        
        }

       
        // Set the table to its original position.....
        swap(s, num1, num2);

        // if new_distance < original_distance ... insert 

        // if (new_distance < original_distance) {
            pair_none_min_distance.push_back(num1);
            pair_none_min_distance.push_back(num2);
           // pair_none_min_distance.push_back(it->second);
            none_min_distance.insert(make_pair(none_min_distance_value, pair_none_min_distance));
            pair_none_min_distance.clear();
        // }
        new_distance = 0; 
        original_distance = 0;
    }
    // else {
    //     cout << "HEREEEEEEEE NUMBEEEEEEERSSSSS IN SET num1 " << num1 << endl; 
    //     cout << "HEREEEEEEEE NUMBEEEEEEERSSSSS IN SET num2 " << num2 << endl; 
    //     cout << "HEREEEEEEEE NUMBEEEEEEERSSSSS IN SET it->second " << it->second << endl; 
    // }
    it++;
    }
}

void enumerate_pairs_closest_distance(Superball *&s, Disjoint_Set &d, multimap<int, int, greater<int> > &djs_sizes_descending_order,  multimap<int, vector <int> > &scores_min_distances, multimap<int, vector <int> > &none_min_distance) {
    // Disjoint_Set d2;
    // multimap<int, vector <int> > scores_min_distances; // the minimum are the best... the closest.. 
    // multimap<int, vector <int> >::iterator scores_min_distances_it;


    // LEAVE IT LIKE THIS SO FAR AND SEE WHAT HAPPENS......well... lets see....!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // multimap<int, vector <int> > closest_of_closest; // the minimum are the best... the closest.. 
    // multimap<int, vector <int> >::iterator closest_of_closest_it;
    // int even_closer_distance;
    // vector <int> swap_pair; 
    // multimap<int, vector<int>, greater<int> > greatest_score_of_each_set;
    // multimap<int, vector<int>, greater<int> >::iterator greatest_score_of_each_set_it; 
    
    int start = 0; 
    for (int i = start; i < s->r*s->c; i++) { 
        for (int j = (start+1); j < s->r*s->c; j++) {
            //cout << i << " " << j << ", ";
            if (isalpha((char)s->board[i]) && isalpha((char)s->board[j]) && ((char)s->board[i] != (char)s->board[j])) {
                closest_distance(s,d,i,j,scores_min_distances, none_min_distance,djs_sizes_descending_order);
            }
        }
        start++;
    }

    // for (scores_min_distances_it = scores_min_distances.begin(); scores_min_distances_it != scores_min_distances.end(); scores_min_distances_it++) {
    //   cout << "score " << scores_min_distances_it->first; 
    //   cout << " pairs ";
    //   for (int i = 0; i < scores_min_distances_it->second.size(); i++) {
    //     cout << scores_min_distances_it->second.at(i) << " * "; //////////////
    //   }
    //   cout << endl;
    // }

    // LEAVE IT LIKE THIS SO FAR AND SEE WHAT HAPPENS......well... lets see....!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // scores_min_distances_it = scores_min_distances.begin();
    // closest_distance = scores_min_distances_it->first;

    // while (scores_min_distances_it->first == closest_distance) {
      
    //   scores_min_distances_it++;
    // }
}

void enumerate_pairs(Superball *&s, Disjoint_Set &d, multimap<int, vector <int>, greater<int> > &all_scores, int greatest_set_in_scoring_cell, int total_size_in_scoring_cell) {
    Disjoint_Set d2;
    // multimap<int, vector <int>, greater<int> > all_scores; // the minimum are the best... the closest.. 
    multimap<int, vector <int>, greater<int> >::iterator all_scores_it;
    vector <int> swap_pair; 
    multimap<int, vector<int>, greater<int> > greatest_score_of_each_set;
    multimap<int, vector<int>, greater<int> >::iterator greatest_score_of_each_set_it; 
    

    // I need to pass this to the analyze function, and insert as I go...
    // Ennumerate pair
    int start = 0; 
    for (int i = start; i < s->r*s->c; i++) { // replace with... s->r*s->c
        for (int j = (start+1); j < s->r*s->c; j++) {
            //cout << i << " " << j << ", ";
            if (isalpha((char)s->board[i]) && isalpha((char)s->board[j]) && ((char)s->board[i] != (char)s->board[j])) {
              // analyze_pairs(s, d, djs_sizes_descending_order); 
              // cout << i << " = " << (char)s->board[i] << " && " <<  j << " = " << (char)s->board[j] << "\n"; 
              // create_sets(s, d, i, j);
              swap(s, i, j);
              //print_board(s);
              create_sets(s, d2); 
              score_each_swap(s,d2,i,j,all_scores, greatest_set_in_scoring_cell, total_size_in_scoring_cell);
              // run score.. this would return an iterator with the greatest score, and the cel of that score.. 
              // then insert in a multimap of scores, swap this score and the swap... all_scores.insert();
              // d2.Print();
              // d.Print();
              // score this table.....

              // If it's empty... another action (closest......)
              swap(s, i, j);
              //cout << endl; 
              //print_board(s);
            }
            // pass i and j to the analyze function.. 
            // So, I can first pass i j.. and then j i???????????????? 
            //cout << endl; 
        }
        //    cout << endl; 
        start++;
    }

    // for (all_scores_it = all_scores.begin(); all_scores_it != all_scores.end(); all_scores_it++) {
    //   cout << "score " << all_scores_it->first; 
    //   cout << " pairs ";
    //   for (int i = 0; i < all_scores_it->second.size(); i++) {
    //     cout << all_scores_it->second.at(i) << " ";
    //   }
    //   cout << endl;
    // }


    // multimap<int, int, greater<int> > djs_sizes_descending_order; // size, index, descending order... 
    // multimap<int, int, greater<int> >::iterator it; 

    // for (int i = 0; i < d.Get_Sizes()->size(); i++) {
    //     //int root = d.Get_Set_Ids()->at(i); 
    //     djs_sizes_descending_order.insert(make_pair(d.Get_Sizes()->at(i), i));
    // }

    // analyze_pairs(s, d, djs_sizes_descending_order); 
    // for (it = djs_sizes_descending_order.begin(); it != djs_sizes_descending_order.end(); it++) {
    //   cout << "it->first " << it->first << "    it->second " << it->second << endl; 
    // }  
}

int main(int argc, char **argv)
{
    Superball *s;
    s = new Superball(argc, argv);
    Disjoint_Set d;
    create_sets(s, d); 

    multimap<int, vector<int>, greater<int> > multimap_scores = is_there_something_to_score(s, d); 
    multimap<int, vector<int>, greater<int> >::iterator is_there_something_to_score_it; 
    is_there_something_to_score_it = multimap_scores.begin();

    // If multimap no empty ...
    if (is_there_something_to_score_it != multimap_scores.end()) {
      // cout << "greatest score " << is_there_something_to_score_it->first;
      // cout << "cells ";
      cout << "SCORE";
      for (int i = 0; i < is_there_something_to_score_it->second.size(); i++) {
        cout << " " << is_there_something_to_score_it->second.at(i); 
    }

    }

    else {
        multimap<int, vector <int>, greater<int> > get_initial_score;
        multimap<int, vector <int>, greater<int> >::iterator get_initial_score_it;
        multimap<int, vector <int>, greater<int> > all_scores; 
        multimap<int, vector <int>, greater<int> >::iterator all_scores_it;
        int initial_score;
        int max_new_score;
        int i = 0, j = 0; 
        // cout << "Go to next step, score each swap... " << endl;
        // cout << "INITIAAAAAAAAL SCORESSSSS " << endl;

        multimap<int, int, greater<int> > djs_sizes_descending_order; // size, index, descending order... 
        multimap<int, int, greater<int> >::iterator djs_sizes_descending_order_it; 
        int greatest_set_in_scoring_cell; 
        int total_size_in_scoring_cell = 0;

        // for (int i = 0; i < s->r*s->c; i++) {
        //     if (s->goals[i] && isalpha((char)s->board[i])) {
        //         int size = d.Get_Sizes()->at(d.Find(i));
        //         djs_sizes_descending_order.insert(make_pair(size, d.Find(i))); // this is gonna be my greatest sizee... the first one.. 
        //     }
        // }

        list <int>::const_iterator eit;
        int root;
        for (int i = 0; i < d.Get_Set_Ids()->size(); i++) {
        root = d.Get_Set_Ids()->at(i); 
        // if (d.Get_Elements()->at(root).empty()) {
        //     printf("root = %d\n", root);
        //     throw std::logic_error("Disjoint_Set::Print() - empty=0");
        // }
            for (eit = d.Get_Elements()->at(root).begin(); eit != d.Get_Elements()->at(root).end(); eit++) {
                //if (((*eit/s->c >= 2) && (*eit/s->c < (s->r - 2))) && ((*eit%s->c < 2) || (*eit%s->c >= (s->c - 2)))) {
                  if (s->goals[*eit/*((*eit/s->c)*(s->c)) + (*eit%s->c)*/] == 1) {
                    if (isalpha((char)s->board[*eit])) {
                      // printf("%2sSize:%3d%2sChar:%2c%2sScoring Cell:%3d,%d", "", (int)d.Get_Elements()->at(root).size(),"",(char)s->board[*eit],"",*eit/s->c,*eit%s->c);
                      int size = d.Get_Sizes()->at(root);
                      djs_sizes_descending_order.insert(make_pair(size, root));
                      break;
                    }
                }
            }
        
    }

    /*
    gg..b....p
......r...
**p.....**
*Bbrp...**
**...pbbBP
**p...y.**
....yy..r.
..gg.yr.g.
    
    */

        for (djs_sizes_descending_order_it = djs_sizes_descending_order.begin(); djs_sizes_descending_order_it != djs_sizes_descending_order.end(); djs_sizes_descending_order_it++) {
            // cout << djs_sizes_descending_order_it->first << " " << djs_sizes_descending_order_it->second << endl; 
            total_size_in_scoring_cell+= djs_sizes_descending_order_it->first;
        }
        djs_sizes_descending_order_it = djs_sizes_descending_order.begin();
        greatest_set_in_scoring_cell  = djs_sizes_descending_order_it->first;

       // cout << "num1 " << num1 << "num2 " << num2 << endl; 
        // cout << "greatest_set_in_scoring_cell  " << greatest_set_in_scoring_cell  << endl;
        // cout << "total_size_in_scoring_cell  " << total_size_in_scoring_cell << endl;

        score_each_swap(s,d,i,j,get_initial_score,greatest_set_in_scoring_cell, total_size_in_scoring_cell);
        get_initial_score_it = get_initial_score.begin();
        initial_score = get_initial_score_it->first;
        // cout << "initial_score " << initial_score << endl; 

        enumerate_pairs(s, d, all_scores,greatest_set_in_scoring_cell, total_size_in_scoring_cell);
        all_scores_it = all_scores.begin();
        max_new_score = all_scores_it->first;
        // cout << "new max score " << max_new_score << endl; 

        // for (all_scores_it = all_scores.begin(); all_scores_it != all_scores.end(); all_scores_it++) {
        // cout << "score " << all_scores_it->first; 
        // cout << " pairs ";
        // for (int i = 0; i < all_scores_it->second.size(); i++) {
        //     cout << all_scores_it->second.at(i) << " ";
        // }
        // cout << endl;
        // }




        all_scores_it = all_scores.begin();
        if (initial_score < max_new_score) { // maybe check here if max_new_score == 0 (no groups of more than 1)... however, 
        //cout << "all_scores " << endl; 
        cout << "SWAP";
        for (int i = 0; i < all_scores_it->second.size(); i++) {
            cout << " " << all_scores_it->second.at(i)/s->c << " " << all_scores_it->second.at(i)%s->c; 
        }
        }
        else {
            // cout << "look for minimum distance " << endl; 
            multimap<int, vector <int> > scores_min_distances; // the minimum are the best... the closest.. 
            multimap<int, vector <int> >::iterator scores_min_distances_it;
            multimap<int, int, greater<int> > djs_sizes_descending_order; // size, index, descending order... 
            multimap<int, int, greater<int> >::iterator it; 
            multimap<int, vector <int> > none_min_distance; 
            multimap<int, vector <int> >::iterator none_min_distance_it;

            for (int i = 0; i < d.Get_Set_Ids()->size(); i++) {
             // cout << d.Get_Set_Ids()->at(i) << " "; 
              int root = d.Get_Set_Ids()->at(i); 
              // cout << root << endl; 
              if (isalpha((char)s->board[root])) {
                int size = d.Get_Sizes()->at(root);
                djs_sizes_descending_order.insert(make_pair(size, root));
              }
            }

            // cout << "\n\n";
            // for (it = djs_sizes_descending_order.begin(); it != djs_sizes_descending_order.end(); it++) {
            //   cout << "it->first " << it->first << "    it->second " << it->second << endl; 
            // }

            enumerate_pairs_closest_distance(s, d, djs_sizes_descending_order, scores_min_distances, none_min_distance);

            // for (scores_min_distances_it = scores_min_distances.begin(); scores_min_distances_it != scores_min_distances.end(); scores_min_distances_it++) {
            // cout << "score " << scores_min_distances_it->first; 
            // cout << " pairs ";
            // for (int i = 0; i < scores_min_distances_it->second.size(); i++) {
            //     cout << scores_min_distances_it->second.at(i) << " * "; //////////////
            // }
            // cout << endl;
            // }

            // I guess, if it's NOT Empty ... swap....
            scores_min_distances_it = scores_min_distances.begin();
            if (scores_min_distances_it != scores_min_distances.end()) {
              // cout << "scores_min_distances" << endl; 
                cout << "SWAP";
                for (int i = 0; i < scores_min_distances_it->second.size(); i++) {
                    cout << " " << scores_min_distances_it->second.at(i)/s->c << " " << scores_min_distances_it->second.at(i)%s->c; 
                }
            }
            else {
                // cout << "none min distance, swap random vals... " << endl;
                none_min_distance_it = none_min_distance.begin();
                if (none_min_distance_it != none_min_distance.end()) {
                  // cout << "none_min_distance " << endl; 
                    cout << "SWAP";
                    for (int i = 0; i < none_min_distance_it->second.size(); i++) {
                        cout << " " << none_min_distance_it->second.at(i)/s->c << " " << none_min_distance_it->second.at(i)%s->c; 
                    }
                }
                else {
                    // swap quite literally any random value no empty.. 
                    // cout << "random value " << endl; 
                    int val1, val2;
                    int start = 0; 
                    for (int i = start; i < s->r*s->c; i++) { // replace with... s->r*s->c
                        for (int j = (start+1); j < s->r*s->c; j++) {
                        //cout << i << " " << j << ", ";
                            if (isalpha((char)s->board[i]) && isalpha((char)s->board[j]) && ((char)s->board[i] != (char)s->board[j])) {
                                // analyze_pairs(s, d, djs_sizes_descending_order); 
                                // cout << i << " = " << (char)s->board[i] << " && " <<  j << " = " << (char)s->board[j] << "\n"; 
                                val1 = i; 
                                val2 = j;
                                goto finish;
                            }
                        }
                        //    cout << endl; 
                        //break; 
                        // cout << "hello" << endl;
                        start++;
                    }
                    finish:
                        cout << "SWAP " << val1/s->c << " " << val1%s->c << " " << val2/s->c << " " << val2%s->c;
                }
            }
        }
        // I guess that ultimately randomly swap two variables.....  see case I drew 
        /**
        * ....rr...... just if I run into issuesss.!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
           ..p......y.... none swap is beneficial... so randomly swap two variable that dont destroy set I guess..... 
        * 
        */
    }
   
//     multimap<int, vector <int>, greater<int> > all_scores;
//     multimap<int, vector <int>, greater<int> >::iterator all_scores_it;
//     int i = 0, j = 0; 

// cout << "INITIAAAAAAAAL SCORESSSSS " << endl;
//     score_each_swap(s,d,i,j,all_scores);
//     for (all_scores_it = all_scores.begin(); all_scores_it != all_scores.end(); all_scores_it++) {
//       cout << "score " << all_scores_it->first; 
//       cout << " pairs ";
//       for (int i = 0; i < all_scores_it->second.size(); i++) {
//         cout << all_scores_it->second.at(i) << " ";
//       }
//       cout << endl;
//     }



//     cout << endl;  
//     d.Print();
//     cout << endl; 
//     enumerate_pairs(s, d);
//     d.Print();
//     cout << endl; 

//     // d.Print();


    // Create sets in descending order... this would help for the third part... closest distance....!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // 
    // multimap<int, int, greater<int> > djs_sizes_descending_order; // size, index, descending order... 
    // multimap<int, int, greater<int> >::iterator it; 

    // for (int i = 0; i < d.Get_Set_Ids()->size(); i++) {
    //  // cout << d.Get_Set_Ids()->at(i) << " "; 
    //   int root = d.Get_Set_Ids()->at(i); 
    //   // cout << root << endl; 
    //   if (isalpha((char)s->board[root])) {
    //     int size = d.Get_Sizes()->at(root);
    //     djs_sizes_descending_order.insert(make_pair(size, root));
    //   }
    // }

    // cout << "\n\n";
    // for (it = djs_sizes_descending_order.begin(); it != djs_sizes_descending_order.end(); it++) {
    //   cout << "it->first " << it->first << "    it->second " << it->second << endl; 
    // }

    // cout << "\n\n";
    // d.Print();
    // cout << "\n\n";

    // enumerate_pairs_closest_distance(s, d, djs_sizes_descending_order);


    // cout << "Making sure I have not changed anything in my original table.....\n\n";
    // d.Print();
    // cout << "\n\n";

    // Create sets in descending order... this would help for the third part... closest distance....!!!!!!!!!!!!!!!!!!!



    // multimap<int, vector <int> > scores_min_distances; // the minimum are the best... the closest.. 
    // multimap<int, vector <int> >::iterator scores_min_distances_it;
    // int num1 = 33;
    // int num2 = 43;
    // closest_distance(s, d, num1, num2, scores_min_distances, djs_sizes_descending_order);

    // for (scores_min_distances_it = scores_min_distances.begin(); scores_min_distances_it != scores_min_distances.end(); scores_min_distances_it++) {
    //   cout << "score " << scores_min_distances_it->first; 
    //   cout << " pairs ";
    //   for (int i = 0; i < scores_min_distances_it->second.size(); i++) {
    //     cout << scores_min_distances_it->second.at(i) << " "; //////////////
    //   }
    //   cout << endl;
    // }



  //   d.Initialize(s->r*s->c);

  // for (int i = 0; i < s->r; i++) {
  //       for (int j = 0; j < s->c; j++) {
  //           if (isalpha((char)s->board[i*s->c+j])) {
  //               if (j < (s->c-1)) {
  //                   if (tolower((char)s->board[i*s->c+(j+1)]) == tolower((char)s->board[i*s->c+j])) {
  //                       s1 = (d.Find(i*s->c+(j)));
  //                       s2 = (d.Find(i*s->c+(j+1)));
  //                       if (s1 != s2)  d.Union(s1, s2);
  //                   }
  //               }
  //               // So, apparently I dont need to check this... why???????????????????????????????????????????????????????????
  //               if (i < s->r-1) {
  //                   if (tolower((char)s->board[(i+1)*s->c + j]) == tolower((char)s->board[i*s->c+j])) {
  //                       s1 = (d.Find(i*s->c+(j)));
  //                       s2 = (d.Find((i+1)*s->c + j));
  //                       if (s1 != s2)  d.Union(s1, s2);
  //                   }
  //               }
  //           }
  //       }
  //   }

    // cout << "\n\n";
    // d.Print();
    // cout << "\n\n";

  // enumerate_pairs(s, d);

//   Disjoint_Set d2 = *d; 
//   Superball su2 = *s;

//   d2.Print();

//   su2.board[8] = (int)'r'; // swap index, swap character... 
//   d2.Union(d2.Find(8), d2.Find(2));

//   for (int i = 0; i < s->board.size(); i++) {
//     //cout << "i " << i << " "; 
//     cout << (char)s->board.at(i) << " ";
//     if ((i+1)%6 == 0) cout << endl; 
//   }

// cout << endl; 
//   for (int i = 0; i < su2.board.size(); i++) {
//     cout << (char)su2.board.at(i) << " ";
//     if ((i+1)%6 == 0) cout << endl; 
//   }

//   d.Print();
//   d2.Print();


  //  for (int i = 0; i < d.Get_Set_Ids()->size(); i++) {
  //       int root = d.Get_Set_Ids()->at(i); 
  //       cout << "root " << root << endl;   
  //       cout << "size of set " << d.Get_Sizes()->at(root) << endl; 
  //  }


  // vector <int> a; 
  // a.push_back(2); 
  // a.push_back(4); 
  // a.push_back(1);
  // a.push_back(0);

  // sort(a.begin(), a.end(), greater <>());
  //  for (int i = 0; i < a.size(); i++) {
  //       //int root = d.Get_Set_Ids()->at(i); 
  //       cout << "i " << i << " // ";   
  //       cout << "size of set " << a.at(i) << endl; 
  //   }

  //   cout << "*std::max_element(a.begin(), a.end())  " << *(a.begin()) << endl; 
  //   cout << "*(d.Get_Sizes()->begin())  " << *std::max_element(d.Get_Sizes()->begin(), d.Get_Sizes()->end()) << endl; 

    // Anyway .. if I order it, I'll lose the index...and I dont want that.. 
    // sort(d.Get_Sizes()->begin(), d.Get_Sizes()->end(), greater <>());

    // for (int i = 0; i < d.Get_Sizes()->size(); i++) {
    //     //int root = d.Get_Set_Ids()->at(i); 
    //     djs_sizes_descending_order.insert(make_pair(d.Get_Sizes()->at(i), i));
    // }

    // for (it = djs_sizes_descending_order.begin(); it != djs_sizes_descending_order.end(); it++) {
    //   cout << "it->first " << it->first << "    it->second " << it->second << endl; 
    // }


    // *std::max_element(scores.begin(), scores.end() )

    //  for (eit = d.Get_Elements()->at(root).begin(); eit != d.Get_Elements()->at(root).end(); eit++) {
    //             //if (((*eit/s->c >= 2) && (*eit/s->c < (s->r - 2))) && ((*eit%s->c < 2) || (*eit%s->c >= (s->c - 2)))) {
    //               if (s->goals[*eit]) {
    //                 cout << endl;
    //                 if (isalpha((char)s->board[*eit])) {

    // is_there_something_to_score(s, d); 

    // multimap<int, vector<int>, greater<int> > highest_scoring_set = is_there_something_to_score(s, d); 
    // multimap<int, vector<int>, greater<int> >::iterator it_multimap; 

    // for (it_multimap = highest_scoring_set.begin(); it_multimap != highest_scoring_set.end(); it_multimap++) {
    //   cout << "score " << it_multimap->first << " row / col "; 
    //   for (int i = 0; i < it_multimap->second.size(); i++) {
    //     cout << it_multimap->second.at(i) << " "; 
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