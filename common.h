#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <string>

#include <boost/algorithm/string.hpp>
#include <boost/dynamic_bitset.hpp>

using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::list;
using std::string;
using std::ifstream;
using boost::dynamic_bitset;

extern void trimAndSplit(string target, string delim,
		list<string> &list_string);

extern void defaultSettings(int &tmp_NUM_CHOICES, vector<int> &scores);

// ざっくりとした上界を計算
extern int getUpperBound(int nd, int np, const vector<int> &scores,
		const vector<vector<int> > &choices, const vector<int> &capacity);

// ２つのベクトルのユークリッド距離を計算する
extern double computeDistance(const vector<int> &a, const vector<int> &b);

// ベクトルの中身を列挙する
extern void showVector(const vector<int> &v);

// 入力ファイルを開く
extern void openInputFile(char* inputFileName, ifstream &ifs);

// 入力ファイルから必要な情報を読み込む
extern void readData(ifstream &ifs, const int NUM_CHOICES,
		const vector<int> &scores, int &numPeople, int &numDept,
		vector<int> &capacity, vector<vector<int> > &choices,
		vector<int> &choicesID);
