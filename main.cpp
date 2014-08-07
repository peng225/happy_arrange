#include <assert.h>
#include <stdlib.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <list>
#include <map>
#include <limits>
#include "node.h"

using std::cout;
using std::cerr;
using std::endl;
using std::cin;
using std::list;
using std::map;
using std::pair;
using std::begin;
using std::end;

void removeInferiorNode(list<Node> &q, const multiset<int> &depts)
{
  for(list<Node>::iterator i = begin(q);
      i != end(q); i++){
    if(i->getDepts() == depts){
      i = q.erase(i);
      i--;
    }
  }
}

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

double computeDistance(vector<int> a, vector<int> b)
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

// template<typename T>
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


int main(int argc, char** args)
{
  //コマンドライン引数の数をチェック  
  // 志望数と各志望度のスコアをセット
  int tmp_NUM_CHOICES;
  vector<int> scores;
  if(argc != 1){
    tmp_NUM_CHOICES = atoi(args[1]) + 1;
    if(argc != tmp_NUM_CHOICES + 2){
      cerr << "Error: Wrong number of point arguments." << endl;
      return 1;
    }

    scores.resize(tmp_NUM_CHOICES);
  
    for(int i = 0; i < (int)scores.size(); i++){
      scores[i] = atoi(args[i + 2]);
      if(scores[i] < 0){
	cerr << "A score must be a positive number." << endl;
	return 1;
      }
    }

    for(int i = 0; i < (int)scores.size() - 1; i++){
      if(scores[i] < scores[i + 1]){
	cerr << "Wrong order between scores." << endl;
	return 1;
      }
    }
  }else{
    tmp_NUM_CHOICES = 3;
    scores.resize(tmp_NUM_CHOICES + 1);
    scores[0] = 5;
    scores[1] = 3;
    scores[2] = 2;
    scores[3] = 0;
  }

  const int NUM_CHOICES = tmp_NUM_CHOICES;
  
  // 人数を読み込む
  int tmp_NUM_PEOPLE;
  cin >> tmp_NUM_PEOPLE;
  const int NUM_PEOPLE = tmp_NUM_PEOPLE;
  
  // 部署数を読み込む
  int tmp_NUM_DEPT;
  cin >> tmp_NUM_DEPT;
  const int NUM_DEPT = tmp_NUM_DEPT;
  cout << tmp_NUM_PEOPLE << ", " << tmp_NUM_DEPT << endl;


  vector<int> capacity(NUM_DEPT);   
  for(vector<int>::iterator i = begin(capacity); i != end(capacity); i++){
    // 各部署の定員を読み込む
    cin >> *i;
  }
  cout << "capacity:" << endl;
  showVector(capacity);

  vector<vector<int> > choices(NUM_PEOPLE);
  vector<int> choicesID(NUM_PEOPLE);

  for(vector<vector<int> >::iterator i = begin(choices);
      i != end(choices); i++){
    i->resize(NUM_DEPT);
  }

  for(int i = 0; i < NUM_PEOPLE; i++){
    choicesID.at(i) = i;
  }

  // for(int i = 0; i < (int)choices.size(); i++){
  for(vector<vector<int> >::iterator i = begin(choices);
      i != end(choices); i++){
    for(int j = 0; j < NUM_CHOICES; j++){
      // 各人のj番目の志望を読み込む
      int dept;
      cin >> dept;
      i->at(dept) = scores.at(j);
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

  // multiset<int> a, b;
  // a.insert(10); b.insert(10);
  // assert(a == b);
  // b.insert(11);
  // assert(a != b);
  // multiset<int> c;
  // c.insert(10);
  // c.insert(11);
  // assert(b == c);
  // c.erase(11);
  // assert(a == c);
  

  cout << "Algorithm start!" << endl;  
  
  // 探索深さdが人数より小さい間
  for(int d = 0; d < NUM_PEOPLE; d++)
  {
    cout << endl;
    cout << "depth : " << d << endl;
    // 全志望度ベクトルの重心に最も近い志望度ベクトルを選ぶ
    pair<int, vector<int> > targetInfo
      = getNearestVector(center, begin(choices) + d, end(choices));
    // これから注目するベクトル
    vector<int> target = targetInfo.second;

    // 選択したベクトルをchoicesから削除
    // choices.erase(begin(choices) + targetInfo.first);
    std::swap(choices.at(d), choices.at(d + targetInfo.first));
    std::swap(choicesID.at(d), choicesID.at(d + targetInfo.first));

    cout << choicesID.at(d) << " : ";
    showVector(target);

    // listが空でなく、かつ先頭要素の深さがdである間
    while(!q.empty() && q.front().getDepth() == d){
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
	  // 履歴を更新
	  newNode.addHistory(i);
	  // 自分が持つ部署集合のスコアを記録
	  // cout << "i : " << i << endl;
	  cout << "new : ";
	  showVector(newNode.getHistory());

	  if(node.getDepts().empty()){
	    score_table[newNode.getDepts()] = target.at(i);
	  }else{
	    score_table[newNode.getDepts()]
	      = score_table[node.getDepts()] + target.at(i);
	  }
	  newNode.setScore(score_table[newNode.getDepts()]);
	  cout << "new score: " << newNode.getScore() << endl;
	  // 新規ノードをlistに入れる
	  q.push_back(newNode);
	}
	// 既存のものよりスコアが高い場合は登録	  
	else if(score_table[newNode.getDepts()]
		< score_table[node.getDepts()] + target.at(i)){
	  // 履歴を更新
	  newNode.addHistory(i);
	  // 自分が持つ部署集合のスコアを記録
	  // cout << "i : " << i << endl;
	  cout << "upd : ";
	  showVector(newNode.getHistory());
	  score_table[newNode.getDepts()]
	    = score_table[node.getDepts()] + target.at(i);
	  newNode.setScore(score_table[newNode.getDepts()]);
	  cout << "upd score: " << newNode.getScore() << endl;
	  // 古いノードを削除し、新規ノードをlistに入れる
	  removeInferiorNode(q, newNode.getDepts());
	  q.push_back(newNode);
	}// else{
	//   cout << "i : " << i << " bad" << endl;
	// }
      }
    }   
  }

  // 最もスコアの高いノードを探す
  int score = std::numeric_limits<int>::min();
  vector<int> result;
  while(!q.empty()){
    Node node = q.front();
    q.erase(begin(q));    
    if(score < node.getScore()){
      score = node.getScore();
      result = node.getHistory();
    }
  }

  cout << "IDs :" << endl;
  showVector(choicesID);
  cout << "history :" << endl;
  showVector(result);

  // 空行を挿入
  cout << endl;
  // 全体の幸福度を表示
  cout << "total happiness: " << score << endl;

  return 0;
}
