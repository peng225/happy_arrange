#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <limits>
#include <fstream>
#include <algorithm>

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/variance.hpp>


#include "node.h"

using std::cout;
using std::cerr;
using std::endl;
using std::list;
using std::map;
using std::pair;
using std::begin;
using std::end;
using std::string;
using std::stoi;
using std::ifstream;
using std::swap;
using std::iter_swap;

using namespace boost::accumulators;

// ざっくりとした上界を計算
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


double getStdDev(const list<Node> &q)
{
  assert(!q.empty());
  accumulator_set<int, stats<tag::variance> > acc;
  BOOST_FOREACH(Node n, q) {    
    acc(n.getScore());
  }
  return sqrt(variance(acc));
}

double getMean(const list<Node> &q)
{
  assert(!q.empty());
  accumulator_set<int, stats<tag::variance> > acc;
  BOOST_FOREACH(Node n, q) {    
    acc(n.getScore());
  }
  return mean(acc);
}

void rmInferiorNodes(list<Node> &q, const map<multiset<int>, int> &score_table)
{
  assert(!q.empty());
  for(list<Node>::iterator i = begin(q);
      i != end(q); i++){
    if(score_table.find(i->getDepts()) != end(score_table)
       && i->getScore() < score_table.at(i->getDepts())){
      i = q.erase(i);
      i--;
    }
  }
}

double getCutOffLowerBound(double mean, double sd, int d, int n)
{
  assert(n != 1);
  // return mean - sd * ((double)n / (d + 1) + n - 2) / (n - 1);
  return mean - 2.0 * sd / (d + 1);
}

// 望みのないノードを削除
void rmHopelessNodes(list<Node> &q, int d, int n)
{
  assert(!q.empty());

  cout << "d, q: " << d << ", " << q.size() << endl;  
  accumulator_set<int, stats<tag::variance> > acc;
  BOOST_FOREACH(Node n, q) {    
    acc(n.getScore());
  }
  
  double mn = mean(acc);
  double sd = sqrt(variance(acc));
  double cutOff = getCutOffLowerBound(mn, sd, d, n);

  int rm = 0;
  for(list<Node>::iterator i = begin(q);
      i != end(q); i++){
    if(i->getScore() < cutOff){
      i = q.erase(i);
      i--;
      rm++;      
    }
  }
  cout << "rm: " << rm << endl;
}

// 重心を計算する
vector<int> computeCenter(const vector<vector<int> > &choices)
{
  assert(choices.size() != 0);
  vector<int> center(choices.at(0).size());

  for(int j = 0; j < (int)center.size(); j++){
    int sum = 0;
    for(vector<vector<int> >::const_iterator i = begin(choices);
	i != end(choices); i++){
      sum += i->at(j);
    }
    center.at(j) = sum / choices.size();
  }
  
  return center;
}

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

// centerに最も近いベクトルの情報を返す
pair<int, vector<int> >
getNearestVector(const vector<int> &center,
		 vector<vector<int> >::const_iterator b,
		 vector<vector<int> >::const_iterator e)
{
  pair<int, vector<int> > target_info;
  target_info.second.resize(center.size());

  double dist = std::numeric_limits<double>::max();
  for(vector<vector<int> >::const_iterator i = b;
      i != e; i++){
    double tmp_dist = computeDistance(*i, center);
    if(tmp_dist < dist){
      dist = tmp_dist;      
      target_info.first = distance(b, i);
      target_info.second = *i;
    }
  }

  return target_info;
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

// masterの値を元にfollowerの値をソートする
// アルゴリズムはクイックソートに準ずる
void sortFollowerWithMaster(vector<int>::iterator m_b,
			    vector<int>::iterator m_e,
			    vector<int>::iterator f_b,
			    vector<int>::iterator f_e)
{
  if(distance(m_b, m_e) != distance(f_b, f_e)){
    cerr << "Error: Length of master and follower must be the same." << endl;
    exit(1);
  }
  
  if(distance(m_b, m_e) <= 1){
    return;
  }

  vector<int>::iterator m_l  = m_b;
  vector<int>::iterator m_r = m_e - 1;
  vector<int>::iterator f_l  = f_b;
  vector<int>::iterator f_r = f_e - 1;
  int pivot = *m_r;

  while(distance(m_l, m_r) > 0){
    while(distance(m_l, m_r) > 0 && *m_l < pivot){
      ++m_l;
      ++f_l;
    }
    if(distance(m_l, m_r) == 0){
      break;
    }

    while(distance(m_l, m_r) > 0 && pivot <= *m_r){
      --m_r;
      --f_r;
    }
    if(distance(m_l, m_r) == 0){
      break;
    }

    iter_swap(m_l, m_r);
    iter_swap(f_l, f_r);
  }

  iter_swap(m_l, m_e - 1);
  iter_swap(f_l, f_e - 1);

  sortFollowerWithMaster(m_b, m_l, f_b, f_l);
  sortFollowerWithMaster(m_l + 1, m_e, f_l + 1, f_e);
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


int main(int argc, char** argv)
{
  //コマンドライン引数のチェック  
  // 志望数と各志望度のスコアをセット
  
  int tmp_NUM_CHOICES;
  vector<int> scores;

  defaultSettings(tmp_NUM_CHOICES, scores);  
  
  int opt;
  bool verbose = false;
  char inputFileName[100];
  // getoptの返り値は見付けたオプション
  while((opt = getopt(argc, argv, "hvi:s:"))!=-1){
    switch(opt){
      // 値をとらないオプション
    case 'h':          
      cout << "Have a nice day!" << endl;
      exit(0);
    case 'v':
      verbose = true;
      break;
      
      // 値を取る引数の場合は外部変数optargにその値を格納する
    case 'i':
      // 入力ファイル名の設定
      strcpy(inputFileName, optarg);
      break;
    case 's':
      // 各志望のスコアを設定 
      {	
	string delim(":");
        list<string> list_string;
	scores.resize(0);
	boost::split(list_string, optarg, boost::is_any_of(delim));
	BOOST_FOREACH(string s, list_string) {
	  scores.push_back(stoi(s));	  
        }

	// スコアがすべて正であるかチェック
	BOOST_FOREACH(int num, scores){
	  if(num <= 0){
	    cerr << "Scores must be a positive number." << endl;
	    return 1;
	  }
	}

	// スコアの間の順序が正しいかチェック
	for(int i = 0; i < (int)scores.size() - 1; i++){
	  if(scores[i] < scores[i + 1]){
	    cerr << "Wrong order between scores." << endl;
	    return 1;
	  }
	}

	tmp_NUM_CHOICES = scores.size();

	// 希望から外れた場合のスコアは問答無用で0
	scores.push_back(0);
	
	break;
      }
      /* 以下二つのcaseは意味がないようだ.
	 getoptが直接エラーを出力してくれるから.
	 プログラムを終了するなら意味があるかも知れない */
    case ':':
      /* 値を取る引数に値がなかった場合:を返す. */
      exit(1);
      
      /* getoptの引数で指定されなかったオプションを受け取ると?を返す. */
    case '?':
      exit(1);
    }
  }  

  const int NUM_CHOICES = tmp_NUM_CHOICES;    

  // 入力ファイルを開く
  std::ifstream ifs(inputFileName);  
  if(ifs.fail()){
    std::cerr << "Failed to open the file." << std::endl;
    exit(1);
  }

  string tstr;

  // 人数を読み込む
  int tmp_NUM_PEOPLE;
  getline(ifs, tstr);
  tmp_NUM_PEOPLE = stoi(tstr);  
  const int NUM_PEOPLE = tmp_NUM_PEOPLE;
  
  // 部署数を読み込む
  int tmp_NUM_DEPT;
  getline(ifs, tstr);
  tmp_NUM_DEPT = stoi(tstr);
  const int NUM_DEPT = tmp_NUM_DEPT;
  
  cout << tmp_NUM_PEOPLE << ", " << tmp_NUM_DEPT << endl;


  // 各部署の定員を読み込む
  vector<int> capacity(NUM_DEPT);   
  for(vector<int>::iterator i = begin(capacity); i != end(capacity); i++){    
    getline(ifs, tstr);
    *i = stoi(tstr);
  }
  cout << "capacity:" << endl;
  showVector(capacity);

  // 部署の定員の総和が人数になっているかチェック
  if(std::accumulate(begin(capacity), end(capacity), 0) != NUM_PEOPLE){
    cerr << "Summation of capacities of all department must be equal to the number of people." << endl;
    exit(1);
  }  

  // 志望度ベクトルの定義と初期化
  vector<vector<int> > choices(NUM_PEOPLE);
  vector<int> choicesID(NUM_PEOPLE);

  for(vector<vector<int> >::iterator i = begin(choices);
      i != end(choices); i++){
    i->resize(NUM_DEPT);
  }

  for(int i = 0; i < NUM_PEOPLE; i++){
    choicesID.at(i) = i;
  }

  // 志望度ベクトルの値を読み込む
  for(vector<vector<int> >::iterator i = begin(choices);
      i != end(choices); i++){    
    string delim(" ");  
    list<string> list_string;
    getline(ifs, tstr);        
    boost::split(list_string, tstr, boost::is_any_of(delim));
    if((int)list_string.size() != NUM_CHOICES){
      cerr << "Error: The size of score list is wrong." << endl;
      exit(1);
    }
    
    for(list<string>::iterator j = begin(list_string);
	j != end(list_string); j++){
      // 各人のj番目の志望を読み込む
      int dept = stoi(*j);
      i->at(dept) = scores.at(distance(begin(list_string), j));
    }
  }

  cout << "choices:" << endl;
  for(vector<vector<int> >::iterator i = begin(choices);
      i != end(choices); i++){
    showVector(*i);
  }  


  
  cout << "Algorithm start!" << endl;
    


  vector<int> cap_filled(NUM_DEPT);
  vector<int> result(NUM_PEOPLE);
  vector<int> suspend;
  int score = 0;

  // choicesから一人ずつ選択
  for(int i = 0; i < NUM_PEOPLE; i++) {
    bool handled = false;
    for(int j = 0; j < NUM_CHOICES; j++){
      // もしまだ第j希望の部署に空きがあればそこに入る
      vector<int>::iterator jthChoice;
      jthChoice = find(begin(choices.at(i)), end(choices.at(i)), scores.at(j));
      int ind = distance(begin(choices.at(i)), jthChoice);
      if(cap_filled.at(ind) < capacity.at(ind)){
	result.at(i) = ind;
	score += scores.at(j);
	cap_filled.at(ind)++;
	handled = true;
	break;	
      }
    }
    if(!handled){
      // 空きがなければ保留リストへ
      suspend.push_back(i);
    }
  }

  // 保留になったものを適当に配置する
  for(int i = 0; i < (int)suspend.size(); i++){
    for(int j = 0; j < NUM_DEPT; j++){
      if(cap_filled.at(j) < capacity.at(j)){
	result.at(suspend.at(i)) = j;
	cap_filled.at(j)++;
	break;
      }
    }
  }


  // 空行を挿入
  cout << endl;

  // // ID順に結果をソート
  // sortFollowerWithMaster(begin(choicesID), end(choicesID),
  // 			 begin(result), end(result));

  cout << "result:" << endl;
  showVector(result);

  int upper = getUpperBound(NUM_DEPT, NUM_PEOPLE, scores, choices, capacity);
  cout << "theoretical upper: " << endl << upper << endl;
  
  // 全体の幸福度を表示
  cout << "total happiness: " << endl << score << endl;

  return 0;
}
