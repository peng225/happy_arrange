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

int main(int argc, char** argv) {
	//コマンドライン引数のチェック
	// 志望数と各志望度のスコアをセット

	int numChoices;
	vector<int> scores;

	defaultSettings(numChoices, scores);

	int opt;
	bool verbose = false;
	bool hopelessCut = false;
	char inputFileName[100] = { };
	// getoptの返り値は見付けたオプション
	while ((opt = getopt(argc, argv, "hvci:s:")) != -1) {
		switch (opt) {
		// 値をとらないオプション
		case 'h':
			cout << "Have a nice day!" << endl;
			exit(0);
		case 'v':
			verbose = true;
			break;
		case 'c':
			// 入力ファイル名の設定
			hopelessCut = true;
			break;

			// 値を取る引数の場合は外部変数optargにその値を格納する
		case 'i':
			// 入力ファイル名の設定
			strcpy(inputFileName, optarg);
			break;
		case 's':
			// 各志望のスコアを設定
		{
			const string delim(":");
			list<string> list_string;
			scores.resize(0);
			boost::split(list_string, optarg, boost::is_any_of(delim));
			try {
				BOOST_FOREACH(string s, list_string){
				scores.push_back(stoi(s));
			}
		} catch(std::invalid_argument &e) {
			cerr << "Error: Failed to convert the command line parameter into integer." << endl;
			exit(1);
		}

		// スコアがすべて正であるかチェック
			BOOST_FOREACH(int num, scores){
			if(num <= 0) {
				cerr << "Error: Scores must be a positive number." << endl;
				return 1;
			}
		}

		// スコアの間の順序が正しいかチェック
			for (int i = 0; i < (int) scores.size() - 1; i++) {
				if (scores[i] <= scores[i + 1]) {
					cerr << "Error: Wrong order between scores." << endl;
					return 1;
				}
			}

			numChoices = scores.size();

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

	const int NUM_CHOICES = numChoices;

	// 入力ファイルを開く
	std::ifstream ifs;
	openInputFile(inputFileName, ifs);

	// 必要なデータを読み込む
	int numPeople, numDept;
	vector<int> capacity, choicesID;
	vector<vector<int> > choices;
	readData(ifs, NUM_CHOICES, scores, numPeople, numDept, capacity, choices,
			choicesID);
	const int NUM_PEOPLE = numPeople;
	const int NUM_DEPT = numDept;

	// 部署番号が小さい部署を志望している人を先に処理するようにソート
	sortFollowerWithMaster(begin(choices), end(choices), begin(choicesID),
			end(choicesID));
	vector<int> result;
	int score = 0;
	list<Node> q;
	// アルゴリズムの本体
	q = pdpSearch(scores, capacity, choices, choicesID, verbose, hopelessCut);
	pdpSelect(q, result, score);
	sortFollowerWithMaster(begin(choicesID), end(choicesID), begin(result),
			end(result));

	// 空行を挿入
	cout << endl;

	cout << "result:" << endl;
	showVector(result);

	int upper = getUpperBound(NUM_DEPT, NUM_PEOPLE, scores, choices, capacity);
	cout << "theoretical upper bound: " << endl << upper << endl;

	// 全体の幸福度を表示
	cout << "total happiness: " << endl << score << endl;

	return 0;
}

