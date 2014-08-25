#include <iostream>
#include <vector>

using std::cout;
using std::cerr;
using std::endl;
using std::vector;

// ２つのベクトルのユークリッド距離を計算する
double computeDistance(const vector<int> &a, const vector<int> &b)
{
  if(a.size() != b.size()){
    cerr << "Different size!" << endl;
    exit(1);
  }

  double dist = 0;
  for(int i = 0; i < (int)a.size(); i++){
    dist += (a.at(i) - b.at(i)) * (a.at(i) - b.at(i));
  }
  dist /= a.size();
  return dist;
}

// ベクトルの中身を列挙する
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
