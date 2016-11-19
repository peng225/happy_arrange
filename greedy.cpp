#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <iostream>
#include <string>
#include <vector>
#include <list>
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
#include "common.h"

using std::cout;
using std::cerr;
using std::endl;
using std::list;
using std::begin;
using std::end;
using std::string;
using std::stoi;
using std::ifstream;
using std::swap;
using std::iter_swap;

int main(int argc, char** argv) {
	//コマンドライン引数のチェック
	// 志望数と各志望度のスコアをセット

	int tmp_NUM_CHOICES;
	vector<int> scores;

	defaultSettings(tmp_NUM_CHOICES, scores);

	int opt;
	// bool verbose = false;
	char inputFileName[100];
	// getoptの返り値は見付けたオプション
	// while((opt = getopt(argc, argv, "hvi:s:"))!=-1){
	while ((opt = getopt(argc, argv, "hi:s:")) != -1) {
		switch (opt) {
		// 値をとらないオプション
		case 'h':
			cout << "Have a nice day!" << endl;
			exit(0);
			// case 'v':
			//   verbose = true;
			//   break;

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
			BOOST_FOREACH(string s, list_string){
			scores.push_back(stoi(s));
		}

		// スコアがすべて正であるかチェック
			BOOST_FOREACH(int num, scores){
			if(num <= 0) {
				cerr << "Scores must be a positive number." << endl;
				return 1;
			}
		}

		// スコアの間の順序が正しいかチェック
			for (int i = 0; i < (int) scores.size() - 1; i++) {
				if (scores[i] < scores[i + 1]) {
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
	if (ifs.fail()) {
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
	for (vector<int>::iterator i = begin(capacity); i != end(capacity); i++) {
		getline(ifs, tstr);
		*i = stoi(tstr);
	}
	cout << "capacity:" << endl;
	showVector(capacity);

	// 部署の定員の総和が人数になっているかチェック
	if (std::accumulate(begin(capacity), end(capacity), 0) != NUM_PEOPLE) {
		cerr
				<< "Summation of capacities of all department must be equal to the number of people."
				<< endl;
		exit(1);
	}

	// 志望度ベクトルの定義と初期化
	vector<vector<int> > choices(NUM_PEOPLE);
	vector<int> choicesID(NUM_PEOPLE);

	for (vector<vector<int> >::iterator i = begin(choices); i != end(choices);
			i++) {
		i->resize(NUM_DEPT);
	}

	for (int i = 0; i < NUM_PEOPLE; i++) {
		choicesID.at(i) = i;
	}

	// 志望度ベクトルの値を読み込む
	for (vector<vector<int> >::iterator i = begin(choices); i != end(choices);
			i++) {
		string delim(" ");
		list<string> list_string;
		getline(ifs, tstr);
		boost::split(list_string, tstr, boost::is_any_of(delim));
		if ((int) list_string.size() != NUM_CHOICES) {
			cerr << "Error: The size of score list is wrong." << endl;
			exit(1);
		}

		for (list<string>::iterator j = begin(list_string);
				j != end(list_string); j++) {
			// 各人のj番目の志望を読み込む
			int dept = stoi(*j);
			i->at(dept) = scores.at(distance(begin(list_string), j));
		}
	}

	cout << "choices:" << endl;
	for (vector<vector<int> >::iterator i = begin(choices); i != end(choices);
			i++) {
		showVector(*i);
	}

	vector<int> cap_filled(NUM_DEPT);
	vector<int> result(NUM_PEOPLE);
	vector<int> suspend;
	int score = 0;

	// choicesから一人ずつ選択
	for (int i = 0; i < NUM_PEOPLE; i++) {
		bool handled = false;
		for (int j = 0; j < NUM_CHOICES; j++) {
			// もしまだ第j希望の部署に空きがあればそこに入る
			vector<int>::iterator jthChoice;
			jthChoice = find(begin(choices.at(i)), end(choices.at(i)),
					scores.at(j));
			int ind = distance(begin(choices.at(i)), jthChoice);
			if (cap_filled.at(ind) < capacity.at(ind)) {
				result.at(i) = ind;
				score += scores.at(j);
				cap_filled.at(ind)++;handled
				= true;
				break;
			}
		}
		if (!handled) {
			// 空きがなければ保留リストへ
			suspend.push_back(i);
		}
	}

	// 保留になったものを適当に配置する
	for (int i = 0; i < (int) suspend.size(); i++) {
		for (int j = 0; j < NUM_DEPT; j++) {
			if (cap_filled.at(j) < capacity.at(j)) {
				result.at(suspend.at(i)) = j;
				cap_filled.at(j)++;break
;			}
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
