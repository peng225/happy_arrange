#pragma once

#include "node.h"
#include "common.h"

#include <iostream>
#include <vector>
#include <list>
#include <map>
//#include <set>
#include <algorithm>

#include <boost/algorithm/string.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/variance.hpp>


using std::list;
using std::map;
using std::pair;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;
using std::begin;
using std::end;
using std::swap;
//using std::multiset;


using namespace boost::accumulators;

const double CUT_OFF_COEF = 1.0;
//const double CUT_OFF_COEF = 1.6;


/*
  scoresの平均を求める。
  希望から外れた場合というのは潜在的には
  NUM_DEPT - score.size() + 1個存在するので、
  そのあたりも加味する。
 */
extern double getScoreMean(const vector<int> &scores, int numDept);

/*
  scoresの分散を求める。
  希望から外れた場合というのは潜在的には
  NUM_DEPT - score.size() + 1個存在するので、
  そのあたりも加味する。
 */
extern double getScoreVariance(const vector<int> &scores, int numDept);

// masterの値を元にfollowerの値をソートする
// アルゴリズムはクイックソートに準ずる
extern void sortFollowerWithMaster(vector<int>::iterator m_b,
		vector<int>::iterator m_e,
		vector<int>::iterator f_b,
		vector<int>::iterator f_e);

extern void sortFollowerWithMaster(vector<vector<int> >::iterator m_b,
		vector<vector<int> >::iterator m_e,
		vector<int>::iterator f_b,
		vector<int>::iterator f_e);

// 望みが薄いノードを切り落とすためのしきい値を計算する
extern double getCutOffLowerBound(list<Node> &q,
		double scoreMean, double scoreVariance,
		int d, int numPeople);


// 同じ部署の選択集合を持つもののうち、劣っているノードをすべて削除する
//extern void rmInferiorNodes(list<Node> &q, const map<multiset<int>, int> &score_table);


// Pseudo Dynamic Programmingの探索フェイズ
extern list<Node> pdpSearch(const vector<int> &scores,
		vector<int> &capacity,
		vector<vector<int> > &choices,
		vector<int> &choicesID,
		bool verbose,
		bool hopelessCut);

// Pseudo Dynamic Programmingの選択フェイズ
extern void pdpSelect(list<Node> &q,	  
		vector<int> &result,
		int &score);

// queueに新しい状態を追加する
extern void addNewState(int dept, bool verbose,
		const Node &node, const vector<int> &target,
		Node &newNode,
		map<vector<int>, int> &score_table,
		list<Node> &q,
		bool isNew);


