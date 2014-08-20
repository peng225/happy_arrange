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

double getScoreMean(vector<int> &scores, int numDept)
{
  accumulator_set<int, stats<tag::mean> > acc;  

  // scoresには希望が通らなかった場合のスコア(0)も含まれるので、それを除く
  for(int i = 0; i < (int)(numDept - scores.size()) + 1; i++){
    acc(0);
  }

  for(vector<int>::iterator i = begin(scores);
      i != end(scores); i++){
    acc(*i);
  }
  
  return mean(acc);
}

double getScoreVariance(vector<int> &scores, int numDept)
{
  accumulator_set<int, stats<tag::variance> > acc;  

  // scoresには希望が通らなかった場合のスコア(0)も含まれるので、それを除く
  for(int i = 0; i < (int)(numDept - scores.size()) + 1; i++){
    acc(0);
  }

  for(vector<int>::iterator i = begin(scores);
      i != end(scores); i++){
    acc(*i);
  }
  
  return variance(acc);
}

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

double getCutOffLowerBound(list<Node> &q,
			   double scoreMean, double scoreVariance,
			   int d, int numPeople)
{
  assert(numPeople != 1);

  accumulator_set<int, stats<tag::variance, tag::mean> > acc;
  BOOST_FOREACH(Node n, q) {    
    acc(n.getScore());
  }
  double mn = mean(acc);
  double va = variance(acc);
  double specMn = mn + scoreMean;
  double specSd = sqrt(va + scoreVariance);
  
  return specMn - 2.0 * specSd / (d + 1);
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

  // 全志望度ベクトルの重心を計算
  vector<int> center(NUM_CHOICES);
  center = computeCenter(choices);
  cout << "center:" << endl;
  showVector(center);

  // 枝狩り幅優先探索のためのlistに空のルートノード(深さ0)を入れる
  Node root(0);
  list<Node> q;
  q.push_back(root);

  // これまでに選択した部署の集合をキーとし、スコアをバリューとしたマップ
  map<multiset<int>, int> score_table;  



  
  cout << "Algorithm start!" << endl;



  double scoreMean = getScoreMean(scores, NUM_DEPT);
  double scoreVariance = getScoreVariance(scores, NUM_DEPT);
  
  // 探索深さdが人数より小さい間
  for(int d = 0; d < NUM_PEOPLE; d++)
  {
    if(verbose){
      cout << endl;    
      cout << "depth: " << d << endl;
    }
    // 全志望度ベクトルの重心に最も近い志望度ベクトルを選ぶ
    pair<int, vector<int> > targetInfo
      = getNearestVector(center, begin(choices) + d, end(choices));
    // これから注目するベクトル
    vector<int> target = targetInfo.second;

    // 選択したベクトルをchoicesから削除
    // choices.erase(begin(choices) + targetInfo.first);
    swap(choices.at(d), choices.at(d + targetInfo.first));
    swap(choicesID.at(d), choicesID.at(d + targetInfo.first));

    if(verbose){
      cout << choicesID.at(d) << ": ";
      showVector(target);
    }

    // 実際の処理に入る前に劣っているノードを削除する
    rmInferiorNodes(q, score_table);
    
    // rmHopelessNodes(q, d, NUM_PEOPLE);

    /*
      カットオフ値をここで決めておく。
      この値は現在のqの内容から推測する。
    */
    double cutOff = getCutOffLowerBound(q, scoreMean, scoreVariance,
					d, NUM_PEOPLE);
    
    int numRemoved = 0;

    assert(!q.empty());
    cout << "d, q: " << d << ", " << q.size() << endl;

    // 先頭要素の深さがdである間
    while(q.front().getDepth() == d){
      assert(!q.empty());
      // if(q.front().getScore() < score_table[q.front().getDepts()]){
      // 	q.erase(begin(q));
      // 	continue;
      // }
      Node node = q.front();
      q.erase(begin(q));      
      for(int i = 0; i < NUM_DEPT; i++){
	// 部署iがすでに定員に達していたら
	if(node.getNumDept(i) == capacity.at(i)){
	  continue;
	}
	// 取り出したノードよりも深さを１つ増やした新規ノード情報を作成
	Node newNode = node;
	newNode.incrementDepth();
	// 現在注目している人がi番目の部署を選んだという情報を新規ノードに加える
	newNode.addDept(i);
	
	// もしこれまでに選択された部署の集合が未登録なら登録	
	if(score_table.find(newNode.getDepts()) == end(score_table)){
	  // if(cutOff <= (score_table.find(node.getDepts()) != end(score_table)
	  if(cutOff <= (node.getDepts().empty()
			? 0 : score_table.at(node.getDepts()))
	   + target.at(i)){
	    // 履歴を更新
	    newNode.addHistory(i);
	    // 自分が持つ部署集合のスコアを記録
	    // cout << "i: " << i << endl;
	    if(verbose){
	      cout << "new: ";
	      showVector(newNode.getHistory());
	    }

	    if(node.getDepts().empty()){
	      score_table[newNode.getDepts()] = target.at(i);
	    }else{
	      score_table[newNode.getDepts()]
		= score_table.at(node.getDepts()) + target.at(i);
	    }
	    newNode.setScore(score_table.at(newNode.getDepts()));
	  
	    if(verbose){
	      cout << "new score: " << newNode.getScore() << endl;
	    }
	  
	    // 新規ノードをlistに入れる
	    q.push_back(newNode);	    
	  }else{
	    numRemoved++;
	  }
	}
	// 既存のものよりスコアが高い場合は登録	  
	else if(score_table.at(newNode.getDepts())
		< score_table.at(node.getDepts()) + target.at(i)){
	  if(cutOff <= score_table.at(node.getDepts()) + target.at(i)){
	    // 履歴を更新
	    newNode.addHistory(i);
	    // 自分が持つ部署集合のスコアを記録
	    // cout << "i: " << i << endl;
	    if(verbose){
	      cout << "upd: ";
	      showVector(newNode.getHistory());
	    }
	    score_table[newNode.getDepts()]
	      = score_table.at(node.getDepts()) + target.at(i);
	    newNode.setScore(score_table.at(newNode.getDepts()));
	    if(verbose){
	      cout << "upd score: " << newNode.getScore() << endl;
	    }
	    // 古いノードを削除し、新規ノードをlistに入れる
	    // removeInferiorNode(q, newNode.getDepts());
	    q.push_back(newNode);	    
	  }else{
	    numRemoved++;
	  }
	}// else{
	//   cout << "i: " << i << " bad" << endl;
	// }
      }
    }
    
    cout << "rm: " << numRemoved << endl;
  }

  // この時点でqにはただ１つのノードが残っている
  // assert(q.size() == 1);

  int score = 0; // q.front().getScore();
  vector<int> result; // = q.front().getHistory();
  while(!q.empty()){
    Node node = q.front();
    q.erase(begin(q));    
    if(score < node.getScore()){
      score = node.getScore();
      result = node.getHistory();
    }
  }

  // 空行を挿入
  cout << endl;
  
  // cout << "IDs:" << endl;
  // showVector(choicesID);
  // cout << "history:" << endl;
  // showVector(result);

  // ID順に結果をソート
  sortFollowerWithMaster(begin(choicesID), end(choicesID),
			 begin(result), end(result));

  cout << "result:" << endl;
  showVector(result);

  int upper = getUpperBound(NUM_DEPT, NUM_PEOPLE, scores, choices, capacity);
  cout << "theoretical upper: " << endl << upper << endl;
  
  // 全体の幸福度を表示
  cout << "total happiness: " << endl << score << endl;

  return 0;
}
