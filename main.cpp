#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

#include <iostream>
#include <string>
#include <fstream>

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/variance.hpp>


#include "node.h"
#include "arrange.h"

using std::cout;
using std::cerr;
using std::endl;
using std::begin;
using std::end;
using std::string;
using std::stoi;
using std::ifstream;


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
  
  vector<int> result;
  int score = 0;

  // アルゴリズムの本体
  arrange(scores, capacity, choices, choicesID, result, score, verbose);
    

  // 空行を挿入
  cout << endl;

  cout << "result:" << endl;
  showVector(result);

  int upper = getUpperBound(NUM_DEPT, NUM_PEOPLE, scores, choices, capacity);
  cout << "theoretical upper: " << endl << upper << endl;
  
  // 全体の幸福度を表示
  cout << "total happiness: " << endl << score << endl;

  return 0;
}
