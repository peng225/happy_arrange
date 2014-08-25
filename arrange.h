#pragma once

#include <iostream>
#include <vector>
#include <list>
#include <map>
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


using namespace boost::accumulators;


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

// 望みが薄いノードを切り落とすためのしきい値を計算する
extern double getCutOffLowerBound(list<Node> &q,
				  double scoreMean, double scoreVariance,
				  int d, int numPeople);

// 重心を計算する
extern vector<int> computeCenter(const vector<vector<int> > &choices);


// 同じ部署の選択集合を持つもののうち、劣っているノードをすべて削除する
extern void rmInferiorNodes(list<Node> &q, const map<multiset<int>, int> &score_table);

// centerに最も近いベクトルの情報を返す
extern pair<int, vector<int> >
  getNearestVector(const vector<int> &center,
		   vector<vector<int> >::const_iterator b,
		   vector<vector<int> >::const_iterator e);

// NUM_CHOICES == scores.size()
// NUM_PEOPLE == choices.size()
// NUM_DEPT == choices.front().size()
extern void arrange(const vector<int> &scores,
		    vector<int> &capacity,
		    vector<vector<int> > &choices,
		    vector<int> &choicesID,
		    vector<int> &result,
		    int &score,
		    bool verbose);