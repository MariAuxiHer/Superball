// so far this one has the best version........ 1714.600 for seed 1 for 10 inputs..

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <iostream>
#include <vector>
#include <map>
#include "disjoint_set.hpp" 
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

void score_each_swap(Superball *&s, Disjoint_Set &d, int &num1, int &num2, multimap<int, vector<int>, greater<int> > &all_swap_scoring, int greatest_set_in_scoring_cell, int total_size_in_scoring_cell) {
  	int points_per_set, total_points = 0; 
   	vector <int> swap_pair; 
    list <int>::const_iterator eit;
    int root;
    multimap<int, int, greater<int> > djs_sizes_descending_order; // size, index, descending order... 
    multimap<int, int, greater<int> >::iterator djs_sizes_descending_order_it; 
    int total_size_after_swap = 0; 
    int greatest_set_after_swap; 
  
    for (size_t i = 0; i < d.Get_Set_Ids()->size(); i++) {
    	root = d.Get_Set_Ids()->at(i); 
    	for (eit = d.Get_Elements()->at(root).begin(); eit != d.Get_Elements()->at(root).end(); eit++) {
            if (s->goals[*eit]) {
                if (isalpha((char)s->board[*eit])) {
                    int size = d.Get_Sizes()->at(root);
                    djs_sizes_descending_order.insert(make_pair(size, root));
                    break;
                }
            }
        }
    }

    for (djs_sizes_descending_order_it = djs_sizes_descending_order.begin(); djs_sizes_descending_order_it != djs_sizes_descending_order.end(); djs_sizes_descending_order_it++) {
        total_size_after_swap+= djs_sizes_descending_order_it->first;
    }

    djs_sizes_descending_order_it = djs_sizes_descending_order.begin();
    greatest_set_after_swap  = djs_sizes_descending_order_it->first;

    for (size_t i = 0; i < d.Get_Set_Ids()->size(); i++) {
        root = d.Get_Set_Ids()->at(i); 
        if (d.Get_Elements()->at(root).size() >= 2) {
            eit = d.Get_Elements()->at(root).begin();
            points_per_set = s->colors[(char)s->board[*eit]];
           	points_per_set *= ((int)d.Get_Elements()->at(root).size());

            for (eit = d.Get_Elements()->at(root).begin(); eit != d.Get_Elements()->at(root).end(); eit++) {
                if (s->goals[*eit]) {
                    // if this set is of the greatest size in scoring cell.. multiply by 1000
                    if (total_size_after_swap >= total_size_in_scoring_cell && greatest_set_after_swap > greatest_set_in_scoring_cell) {
                        points_per_set *= 1000;
                        break;
                    }
                    else {
                        points_per_set *= 100;
                        break;
                    }
                }
            }
        	total_points += points_per_set;
        }
 
	}
 
    swap_pair.push_back(num1);
    swap_pair.push_back(num2);
    all_swap_scoring.insert(make_pair(total_points, swap_pair));
}

multimap<int, vector<int>, greater<int> > is_there_something_to_score(Superball *&s, Disjoint_Set &d) {
  	int points_per_set; 
  	multimap<int, vector<int>, greater<int> > highest_scoring_set; 
    list <int>::const_iterator eit;
    int root;

    for (size_t i = 0; i < d.Get_Set_Ids()->size(); i++) {
        root = d.Get_Set_Ids()->at(i); 
        if ((int)d.Get_Elements()->at(root).size() >= s->mss) {
            for (eit = d.Get_Elements()->at(root).begin(); eit != d.Get_Elements()->at(root).end(); eit++) {
                if (s->goals[*eit]) {
                    points_per_set = s->colors[(char)s->board[*eit]];
                    vector <int> scoring_set_cel;  
                    points_per_set *= ((int)d.Get_Elements()->at(root).size());
                    scoring_set_cel.push_back(*eit/s->c);
                    scoring_set_cel.push_back(*eit%s->c);
                    highest_scoring_set.insert(make_pair(points_per_set, scoring_set_cel));
                    break; 
                }
            }
        }
    }
  
    return  highest_scoring_set;
}


void swap(Superball *&s, int &num1, int &num2) {
  int temp = s->board[num1];
  s->board[num1] = s->board[num2];
  s->board[num2] =  temp;
}

int calculate_distance (int &col, int &num1, list<int> set) {
    int min_element = *std::min_element(set.begin(), set.end());
    int max_element = *std::max_element(set.begin(), set.end()); 
    int distance;
    list<int>::iterator it; 
    
    if (num1 < min_element) {
        if (num1%col <= min_element%col) {
            distance = abs(num1%col - min_element%col) + abs(num1/col - min_element/col);
        }
        else {
            distance = -1; 
            for (it = set.begin(); it != set.end(); it++) {
                if (distance == -1 || abs(num1%col - *it%col) + abs(num1/col - *it/col) < distance) {
                    distance = abs(num1%col - *it%col) + abs(num1/col - *it/col);
                }
            }
        }
    }
    else if (num1 > max_element) {

        if (num1%col >= max_element%col) {
            distance = abs(num1%col - max_element%col) + abs(num1/col - max_element/col);
        }
        else {
            distance = -1; 
            for (it = set.begin(); it != set.end(); it++) {
                if (distance == -1 || abs(num1%col - *it%col) + abs(num1/col - *it/col) < distance) {
                    distance = abs(num1%col - *it%col) + abs(num1/col - *it/col);
                }
            }
        }
    }
    else if (num1 > min_element && num1 < max_element) {
        distance = -1; 
        for (it = set.begin(); it != set.end(); it++) {
            if (num1/col == *it/col) {
                if (distance == -1 || abs(num1%col - *it%col) < distance) {
                    distance = abs(num1%col - *it%col);
                }
            }
        }
    }
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
    list<int>::iterator find_num1; 
    list<int>::iterator find_num2; 
    list<int> set;


    while (it != djs_sizes_descending_order.end()) {
       
       set = d.Get_Elements()->at(it->second);
       find_num1 = find(set.begin(), set.end(), num1);
       find_num2 = find(set.begin(), set.end(), num2);

        if ((find_num1 == set.end()) && (find_num2 == set.end()) && (d.Get_Elements()->at(d.Find(num1)).size() < 2) && (d.Get_Elements()->at(d.Find(num2)).size() < 2)) {

        	if (s->board[num1] == s->board[it->second]) {
            	original_distance = calculate_distance (s->c, num1, d.Get_Elements()->at(it->second)); 
        	}
        	if (s->board[num2] == s->board[it->second]) {
            	original_distance = calculate_distance (s->c, num2, d.Get_Elements()->at(it->second)); 
        	}

        	swap(s, num1, num2);

        	if (s->board[num1] == s->board[it->second]) {
            	new_distance = calculate_distance (s->c, num1, d.Get_Elements()->at(it->second)); 
            	if (new_distance < original_distance) { 
            		swap_pair.push_back(num1);
            		swap_pair.push_back(num2);
            		scores_min_distances.insert(make_pair(new_distance, swap_pair));
            		swap_pair.clear();
        		}
       
        	}

        	if (s->board[num2] == s->board[it->second]) {
            	new_distance = calculate_distance (s->c, num2, d.Get_Elements()->at(it->second)); 
     
            	if (new_distance < original_distance) { 
            		swap_pair.push_back(num1);
            		swap_pair.push_back(num2);
           
            		scores_min_distances.insert(make_pair(new_distance, swap_pair));
            		swap_pair.clear();
        		}
        
        	}

        	// Set the table to its original position.....
        	swap(s, num1, num2);

            pair_none_min_distance.push_back(num1);
            pair_none_min_distance.push_back(num2);
            none_min_distance.insert(make_pair(none_min_distance_value, pair_none_min_distance));
            pair_none_min_distance.clear();
    
        	new_distance = 0; 
        	original_distance = 0;
    	}
   
    	it++;
    }
}

void enumerate_pairs_closest_distance(Superball *&s, Disjoint_Set &d, multimap<int, int, greater<int> > &djs_sizes_descending_order,  multimap<int, vector <int> > &scores_min_distances, multimap<int, vector <int> > &none_min_distance) {
    
    int start = 0; 
    for (int i = start; i < s->r*s->c; i++) { 
        for (int j = (start+1); j < s->r*s->c; j++) {
            if (isalpha((char)s->board[i]) && isalpha((char)s->board[j]) && ((char)s->board[i] != (char)s->board[j])) {
                closest_distance(s,d,i,j,scores_min_distances, none_min_distance,djs_sizes_descending_order);
            }
        }
        start++;
    }
}

void enumerate_pairs(Superball *&s, Disjoint_Set &d, multimap<int, vector <int>, greater<int> > &all_scores, int greatest_set_in_scoring_cell, int total_size_in_scoring_cell) {
    Disjoint_Set d2;
    multimap<int, vector <int>, greater<int> >::iterator all_scores_it;
    vector <int> swap_pair; 
    multimap<int, vector<int>, greater<int> > greatest_score_of_each_set;
    multimap<int, vector<int>, greater<int> >::iterator greatest_score_of_each_set_it; 
    
    // Ennumerate pair
    int start = 0; 
    for (int i = start; i < s->r*s->c; i++) { 
        for (int j = (start+1); j < s->r*s->c; j++) {
            if (isalpha((char)s->board[i]) && isalpha((char)s->board[j]) && ((char)s->board[i] != (char)s->board[j])) {
              	swap(s, i, j);
              	create_sets(s, d2); 
              	score_each_swap(s,d2,i,j,all_scores, greatest_set_in_scoring_cell, total_size_in_scoring_cell);
              	swap(s, i, j);
            }
        }
        start++;
    }
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

    if (is_there_something_to_score_it != multimap_scores.end() && s->empty < 6) {
      	cout << "SCORE";
      	for (size_t i = 0; i < is_there_something_to_score_it->second.size(); i++) {
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

        	multimap<int, int, greater<int> > djs_sizes_descending_order; // size, index, descending order... 
        	multimap<int, int, greater<int> >::iterator djs_sizes_descending_order_it; 
        	int greatest_set_in_scoring_cell; 
        	int total_size_in_scoring_cell = 0;
 
        	list <int>::const_iterator eit;
        	int root;
        	for (size_t i = 0; i < d.Get_Set_Ids()->size(); i++) {
        		root = d.Get_Set_Ids()->at(i); 
        
            	for (eit = d.Get_Elements()->at(root).begin(); eit != d.Get_Elements()->at(root).end(); eit++) {
                  	if (s->goals[*eit] == 1) {
                    	if (isalpha((char)s->board[*eit])) {
                      		int size = d.Get_Sizes()->at(root);
                      		djs_sizes_descending_order.insert(make_pair(size, root));
                      		break;
                    	}
                	}
            	}
        
    		}

        for (djs_sizes_descending_order_it = djs_sizes_descending_order.begin(); djs_sizes_descending_order_it != djs_sizes_descending_order.end(); djs_sizes_descending_order_it++) {
            total_size_in_scoring_cell+= djs_sizes_descending_order_it->first;
        }
        djs_sizes_descending_order_it = djs_sizes_descending_order.begin();
        greatest_set_in_scoring_cell  = djs_sizes_descending_order_it->first;

        score_each_swap(s,d,i,j,get_initial_score,greatest_set_in_scoring_cell, total_size_in_scoring_cell);
        get_initial_score_it = get_initial_score.begin();
        initial_score = get_initial_score_it->first;

        enumerate_pairs(s, d, all_scores,greatest_set_in_scoring_cell, total_size_in_scoring_cell);
        all_scores_it = all_scores.begin();
        max_new_score = all_scores_it->first;

        all_scores_it = all_scores.begin();
        if (initial_score < max_new_score) { 
        cout << "SWAP";
        for (size_t i = 0; i < all_scores_it->second.size(); i++) {
            cout << " " << all_scores_it->second.at(i)/s->c << " " << all_scores_it->second.at(i)%s->c; 
        }
        }
        else {
            multimap<int, vector <int> > scores_min_distances; 
            multimap<int, vector <int> >::iterator scores_min_distances_it;
            multimap<int, int, greater<int> > djs_sizes_descending_order;
            multimap<int, int, greater<int> >::iterator it; 
            multimap<int, vector <int> > none_min_distance; 
            multimap<int, vector <int> >::iterator none_min_distance_it;

            for (size_t i = 0; i < d.Get_Set_Ids()->size(); i++) {
              	int root = d.Get_Set_Ids()->at(i); 
              
              	if (isalpha((char)s->board[root])) {
                	int size = d.Get_Sizes()->at(root);
                	djs_sizes_descending_order.insert(make_pair(size, root));
              	}
            }

            enumerate_pairs_closest_distance(s, d, djs_sizes_descending_order, scores_min_distances, none_min_distance);

            scores_min_distances_it = scores_min_distances.begin();
            if (scores_min_distances_it != scores_min_distances.end()) {
                cout << "SWAP";
                for (size_t i = 0; i < scores_min_distances_it->second.size(); i++) {
                    cout << " " << scores_min_distances_it->second.at(i)/s->c << " " << scores_min_distances_it->second.at(i)%s->c; 
                }
            }
            else {
                none_min_distance_it = none_min_distance.begin();
                if (none_min_distance_it != none_min_distance.end()) {
                    cout << "SWAP";
                    for (size_t i = 0; i < none_min_distance_it->second.size(); i++) {
                        cout << " " << none_min_distance_it->second.at(i)/s->c << " " << none_min_distance_it->second.at(i)%s->c; 
                    }
                }
                else {
                    int val1, val2;
                    int start = 0; 
                    for (int i = start; i < s->r*s->c; i++) { 
                        for (int j = (start+1); j < s->r*s->c; j++) {
                            if (isalpha((char)s->board[i]) && isalpha((char)s->board[j]) && ((char)s->board[i] != (char)s->board[j])) {
                                val1 = i; 
                                val2 = j;
                                goto finish;
                            }
                        }
                        start++;
                    }
                    finish:
                        cout << "SWAP " << val1/s->c << " " << val1%s->c << " " << val2/s->c << " " << val2%s->c;
                }
            }
        }
    }
}