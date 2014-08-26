#include "common.h"

void trimAndSplit(string target, string delim,
		  list<string> &list_string)
{
  boost::trim(target);
  boost::split(list_string, target, boost::is_any_of(delim),
	       boost::algorithm::token_compress_on);
}

void defaultSettings(int &tmp_NUM_CHOICES, vector<int> &scores)
{
  tmp_NUM_CHOICES = 3;
  scores.resize(tmp_NUM_CHOICES + 1);
  scores[0] = 5;
  scores[1] = 3;
  scores[2] = 2;
  scores[3] = 0;
}

int getUpperBound(int nd, int np, const vector<int> &scores,
		  const vector<vector<int> > &choices,
		  const vector<int> &capacity)
{  
  int upper = 0;
  for(int i = 0; i < nd; i++){
    int count = 0;
    for(int j = 0; j < np; j++){
      if(choices.at(j).at(i) == scores.at(0)){
	count++;
      }
    }
    // 定員を超える第一志望人員がいたら残念賞
    if(capacity.at(i) < count){
      upper += scores.at(0) * capacity.at(i);
      upper += scores.at(1) * (count - capacity.at(i));
    }else{
      upper += scores.at(0) * count;
    }
  }
  return upper;
}

double computeDistance(const vector<int> &a, const vector<int> &b)
{
  if(a.size() != b.size()){
    cerr << "Error: Different size vectors are given." << endl;
    exit(1);
  }

  double dist = 0;
  for(int i = 0; i < (int)a.size(); i++){
    dist += (a.at(i) - b.at(i)) * (a.at(i) - b.at(i));
  }
  dist /= a.size();
  return dist;
}

void showVector(const vector<int> &v)
{
  for(vector<int>::const_iterator i = begin(v); i != end(v); i++){
    cout << *i;
    if(distance(i, end(v)) != 1){
      cout << ", ";
    }else{
      cout << endl;
    }
  }
}
