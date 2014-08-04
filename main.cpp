#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include "node.h"

using std::cout;
using std::cerr;
using std::endl;
using std::cin;
using std::vector;
using std::begin;
using std::end;

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

  for(vector<vector<int> >::iterator i = begin(choices);
      i != end(choices); i++){
    i->resize(NUM_DEPT);
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

  // 幅優先探索のためのqueueに空のルートノード(深さ-1)を入れる
  

  // 探索深さdが人数より小さい間
  for(int d = 0; d < NUM_PEOPLE; d++)
  {
    // これまでに選択したベクトルの数が0なら
    {
      // 全志望度ベクトルの重心に最も近い志望度ベクトルを選ぶ
    }
    // さもなければ
    {
      // これまでに選択したベクトルの重心に最も近い志望度ベクトルを選ぶ
    }    

    // queueが空でなく、かつ先頭要素の深さがdである間
    {
      for(int i = 0; i < NUM_DEPT; i++){
	// 部署iがすでに定員に達していたら
	{
	  continue;
	}
	// 取り出したノードよりも深さを１つ増やした新規ノード情報を作成
	// 現在注目している人がi番目の部署を選んだという情報を新規ノードに加える
	// もしこれまでに選択された部署の集合が持つスコアが既存のものよりも高い場合
	{
	  // 自分が持つ部署集合のスコアを記録
	  // 新規ノードをqueueに入れる
	}
      }
    }
    
    // 今回選ばれた志望度ベクトルを、 選択済みベクトルリストに移す
    // これまでに選ばれた志望度ベクトルの重心を計算
  }

  for(int i = 0; i < NUM_PEOPLE; i++){
    // 各人の配属先を表示
  }
  // 空行を挿入
  // 全体の幸福度を表示

  return 0;
}
