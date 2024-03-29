/******************************************************************************************************************
    Maria Hernandez 
    COSC302
    03/10/2023
    Project3
    play.cpp
    This program tries to determine the best moves in a Superball game. If there is something to score in the table
    and the empty cells are less than 6, it scores. Otherwise, it tries to find the best swap, by accounting for the 
    number of sets in the table, their values (depending on the color), and the number of sets in the scoring cells.
    If there are bo good swaps, it just performs a random swap.
*******************************************************************************************************************/

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

// Function that analyzes the board.
void create_sets (Superball *&s, Disjoint_Set &d) {
    d.Initialize(s->r*s->c);
    int s1, s2;

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
                    if (s->board[i*s->c+(j+1)] == s->board[i*s->c+j]) {
                        s1 = (d.Find(i*s->c+(j)));
                        s2 = (d.Find(i*s->c+(j+1)));
                        if (s1 != s2)  d.Union(s1, s2);
                    }
                }
                
                if (i < s->r-1) {

					// If the cell below the current contain the same element, we find the root node of both, and if they have different
					// roots, we join them. 
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

// Function that score each swap. The function tries to find the best swap, by accounting for the number of sets in the table, 
// their values (depending on the color), and the number of sets in the scoring cells.
void score_each_swap(Superball *&s, Disjoint_Set &d, int &num1, int &num2, multimap<int, vector<int>, greater<int> > &all_swap_scoring, int greatest_set_in_scoring_cell, int total_size_in_scoring_cell) {
  	int points_per_set, total_points = 0; 
   	vector <int> swap_pair; 
    list <int>::const_iterator eit;
    int root;
    multimap<int, int, greater<int> > djs_sizes_descending_order;  
    multimap<int, int, greater<int> >::iterator djs_sizes_descending_order_it; 
    int total_size_after_swap = 0; 
    int greatest_set_after_swap; 
  
	// Create a multimap containing the sizes of all disjoint sets in the scoring cells in the new table (after the swap) as the key, 
	// and the root node of the set as the value.
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

	// Loop through the multimap of all sizes to find the total size of all sets of the new table.
    for (djs_sizes_descending_order_it = djs_sizes_descending_order.begin(); djs_sizes_descending_order_it != djs_sizes_descending_order.end(); djs_sizes_descending_order_it++) {
        total_size_after_swap+= djs_sizes_descending_order_it->first;
    }

	// The element at the beginning of the map will contain the set with the greatest size of the new table.
    djs_sizes_descending_order_it = djs_sizes_descending_order.begin();
    greatest_set_after_swap  = djs_sizes_descending_order_it->first;

	// Score system to score each swap.
	// Loop through the vector returned by Get_Set_Ids(), which contains all the root nodes. 
    for (size_t i = 0; i < d.Get_Set_Ids()->size(); i++) {
        root = d.Get_Set_Ids()->at(i); 
        if (d.Get_Elements()->at(root).size() >= 2) {
		
			// Find the value of the character in the set, given by the colors vector. Then multiple the value by the size of the set. 
            eit = d.Get_Elements()->at(root).begin();
			
            points_per_set = s->colors[(char)s->board[*eit]];
           	points_per_set *= ((int)d.Get_Elements()->at(root).size());

			// Loop that check whether any of the elements in the set is in one of the scoring cell, to give them a greater score.
            for (eit = d.Get_Elements()->at(root).begin(); eit != d.Get_Elements()->at(root).end(); eit++) {
                if (s->goals[*eit]) {

					// If this swap gives me more or the same amount of sets than the original board and in addition the set with 
					// the greatest size is greater than the set with the greatest size in the current board, we give the swap extra points.
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
			
			// Add the points of this set to the running total. 
        	total_points += points_per_set;
        }
	}
 
	// Push back this swap to the multimap containing all swaps and their possible scores. 
    swap_pair.push_back(num1);
    swap_pair.push_back(num2);
    all_swap_scoring.insert(make_pair(total_points, swap_pair));
}

// Function that checks whether there is something to score in the current table.
multimap<int, vector<int>, greater<int> > is_there_something_to_score(Superball *&s, Disjoint_Set &d) {
  	int points_per_set; 
  	multimap<int, vector<int>, greater<int> > highest_scoring_set; 
    list <int>::const_iterator eit;
    int root;

	// Loop through all the root nodes to access each set.
    for (size_t i = 0; i < d.Get_Set_Ids()->size(); i++) {
        root = d.Get_Set_Ids()->at(i); 

		// If the size set is greater than the minimum scoring size, we loop through the set and check whether it contains an element present 
		// in the scoring cells. We then get the value of that set, multiply it by the size and push that element into a multimap containing all 
		// possible scores, this multimap is ordered in descending order.
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

// Function that swaps two cells.
void swap(Superball *&s, int &num1, int &num2) {
  int temp = s->board[num1];
  s->board[num1] = s->board[num2];
  s->board[num2] =  temp;
}

// Function that calculates the distance between an element in an array and a given set.
int calculate_distance (int &col, int &num1, list<int> set) {
    int min_element = *std::min_element(set.begin(), set.end());
    int max_element = *std::max_element(set.begin(), set.end()); 
    int distance;
    list<int>::iterator it; 
    
	// We divide the number passed by the number of columns to get the row number and mod it by the columns to get the col number (in a 2D representation).
    // The distance is given as follow:
	// If the element is in the same column as one of the elements in the set (the closest), we get the absolute difference between the columns.
	// If the element is in the same row as one of the elements in the set (the closest), we get the absolute difference between the rows.
	// Otherwise, we get the absolute difference between the columns and the absolute difference between the rows, and add them.
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

// The following function finds the closest distance from all the possible pairs between an element and any of the sets containing the same letter. 
// We call this function only if from all the swaps, we don't get a swap that will give us a greater number of sets than our current table.
void closest_distance(Superball *&s, Disjoint_Set &d, int &num1, int &num2, multimap<double, vector <int>, greater<double> > &scores_min_distances, multimap<int, vector <int>, greater<int> > &none_min_distance, multimap<int, int, greater<int> > &djs_sizes_descending_order) {
    multimap<int, int, greater<int> >::iterator it; 
    vector <int> swap_pair; 
    vector <int> pair_none_min_distance; 
    int none_min_distance_value = 0; 
    it = djs_sizes_descending_order.begin();
    double original_distance = 0; 
    double new_distance = 0; 
    list<int>::iterator find_num1; 
    list<int>::iterator find_num2; 
    list<int> set;

	// Analyze each swap against all sets in the original table, and see which swap brings an element the closest to a set containing the same character.
    while (it != djs_sizes_descending_order.end()) {
       
       set = d.Get_Elements()->at(it->second);
       find_num1 = find(set.begin(), set.end(), num1);
       find_num2 = find(set.begin(), set.end(), num2);

		// We don't calculate the original distance vs the new distance, if the element passed is part of the set we are currently analyzing, or if we 
		// will "destroy" a set of two or more elements.
        if ((find_num1 == set.end()) && (find_num2 == set.end()) && (d.Get_Elements()->at(d.Find(num1)).size() < 2) && (d.Get_Elements()->at(d.Find(num2)).size() < 2)) {

			// If any of the number swapped contains the same character as the set, we calculate the original distance of that character to the set
			// vs the new distance. If the new distance is lower, we push that swap our multimap.
        	if (s->board[num1] == s->board[it->second]) {
            	original_distance = calculate_distance (s->c, num1, d.Get_Elements()->at(it->second)); 
        	}

        	if (s->board[num2] == s->board[it->second]) {
            	original_distance = calculate_distance (s->c, num2, d.Get_Elements()->at(it->second)); 
        	}

        	swap(s, num1, num2);

        	if (s->board[num1] == s->board[it->second]) {
            	new_distance = calculate_distance (s->c, num1, d.Get_Elements()->at(it->second)); 
            	if (1.0/new_distance > 1.0/original_distance) { 
            		swap_pair.push_back(num1);
            		swap_pair.push_back(num2);
                    new_distance = 1.0/new_distance;
            		scores_min_distances.insert(make_pair(new_distance, swap_pair));
            		swap_pair.clear();
        		}
       
        	}

        	if (s->board[num2] == s->board[it->second]) {
            	new_distance = calculate_distance (s->c, num2, d.Get_Elements()->at(it->second)); 
            	if (1.0/new_distance > 1.0/original_distance) { 
            		swap_pair.push_back(num1);
            		swap_pair.push_back(num2);
                    new_distance = 1.0/new_distance;
            		scores_min_distances.insert(make_pair(new_distance, swap_pair));
            		swap_pair.clear();
        		}
        
        	}

        	// Set the table to its original position.
        	swap(s, num1, num2);

			// If there's no possible new min distance, we just use this vector that - in that case - will contain a random swap.
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

// Find all the possible valid pairs to be used in the closest_distance() function.
void enumerate_pairs_closest_distance(Superball *&s, Disjoint_Set &d, multimap<int, int, greater<int> > &djs_sizes_descending_order,  multimap<double, vector <int>, greater<double> > &scores_min_distances, multimap<int, vector <int>, greater<int> > &none_min_distance) {
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

// Find all the possible valid pairs to be used in the score_each_swap() function.
void enumerate_pairs(Superball *&s, multimap<int, vector <int>, greater<int> > &all_scores, int greatest_set_in_scoring_cell, int total_size_in_scoring_cell) {
    Disjoint_Set d2;
    multimap<int, vector <int>, greater<int> >::iterator all_scores_it;
    vector <int> swap_pair; 
    multimap<int, vector<int>, greater<int> > greatest_score_of_each_set;
    multimap<int, vector<int>, greater<int> >::iterator greatest_score_of_each_set_it; 
    
    // Ennumerate pairs.
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

	// If there is something to score in the current table and if the number of empty cells is less than 6, we find the highest score for the table.
    if (is_there_something_to_score_it != multimap_scores.end() && s->empty < 6) {
      	cout << "SCORE";
      	for (size_t i = 0; i < is_there_something_to_score_it->second.size(); i++) {
        	cout << " " << is_there_something_to_score_it->second.at(i); 
    	}
    }

	// Otherwise, we find the best swap that will give us more number of sets, with more sets in the scoring cells.
    else {
        multimap<int, vector <int>, greater<int> > get_initial_score;
        multimap<int, vector <int>, greater<int> >::iterator get_initial_score_it;
        multimap<int, vector <int>, greater<int> > all_scores; 
        multimap<int, vector <int>, greater<int> >::iterator all_scores_it;
		int greatest_set_in_scoring_cell; 
        int total_size_in_scoring_cell = 0;
        list <int>::const_iterator eit;
        int initial_score;
        int max_new_score;
        int i = 0, j = 0; 

		// Create a multimap containing the sizes of all disjoint sets in the scoring cells in the current table as the key,
		// and the root node of the set as the value.
        multimap<int, int, greater<int> > djs_sizes_descending_order; 
        multimap<int, int, greater<int> >::iterator djs_sizes_descending_order_it; 
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

		// Loop through the multimap of all sizes to find the total size of all sets of the current table.
        for (djs_sizes_descending_order_it = djs_sizes_descending_order.begin(); djs_sizes_descending_order_it != djs_sizes_descending_order.end(); djs_sizes_descending_order_it++) {
            total_size_in_scoring_cell+= djs_sizes_descending_order_it->first;
        }

		// The element at the beginning of the map will contain the set with the greatest size of the new table.
        djs_sizes_descending_order_it = djs_sizes_descending_order.begin();
        greatest_set_in_scoring_cell  = djs_sizes_descending_order_it->first;

		// Call the score_each_swap function to get the initial score of our table. 
        score_each_swap(s,d,i,j,get_initial_score,greatest_set_in_scoring_cell, total_size_in_scoring_cell);
        get_initial_score_it = get_initial_score.begin();
        initial_score = get_initial_score_it->first;

		// Call the enumerate pairs function to find all possible to pass to the function that will analyze all 
		// possible tables after all possible swaps and get the new score.
        enumerate_pairs(s, all_scores,greatest_set_in_scoring_cell, total_size_in_scoring_cell);
        all_scores_it = all_scores.begin();
        max_new_score = all_scores_it->first;
        all_scores_it = all_scores.begin();

		// If the initial score is less than the new score, we swap our best swap obtained with the score_each_swap() function.
        if (initial_score < max_new_score) { 
        	cout << "SWAP";
        	for (size_t i = 0; i < all_scores_it->second.size(); i++) {
            	cout << " " << all_scores_it->second.at(i)/s->c << " " << all_scores_it->second.at(i)%s->c; 
        	}
        }

		// Otherwise, we find the closest distance from all the possible pairs between an element and any of the sets containing the same letter. 
        else {
            multimap<double, vector <int>, greater<double> > scores_min_distances; 
            multimap<double, vector <int>, greater<double> >::iterator scores_min_distances_it;
            multimap<int, int, greater<int> > djs_sizes_descending_order;
            multimap<int, int, greater<int> >::iterator it; 
            multimap<int, vector <int>, greater<int> > none_min_distance; 
            multimap<int, vector <int>, greater<int> >::iterator none_min_distance_it;

			// Create a multimap containing the sizes of all disjoint sets of the current table as the key, and the root node of the set as the value.
            for (size_t i = 0; i < d.Get_Set_Ids()->size(); i++) {
              	int root = d.Get_Set_Ids()->at(i);  
              	if (isalpha((char)s->board[root])) {
                	int size = d.Get_Sizes()->at(root);
                	djs_sizes_descending_order.insert(make_pair(size, root));
              	}
            }

			// Call the function that will find all the possible valid pairs to be used in the closest_distance() function.
            enumerate_pairs_closest_distance(s, d, djs_sizes_descending_order, scores_min_distances, none_min_distance);

			// If we find a new minimum distance, we swap. 
            scores_min_distances_it = scores_min_distances.begin();
            if (scores_min_distances_it != scores_min_distances.end()) {
                cout << "SWAP";
                for (size_t i = 0; i < scores_min_distances_it->second.size(); i++) {
                    cout << " " << scores_min_distances_it->second.at(i)/s->c << " " << scores_min_distances_it->second.at(i)%s->c; 
                }
            }

			// If none of the above works, we just perform a random swap. 
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

						// Loop through the table and as soon as we find a valid swap pair, we swap it.
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