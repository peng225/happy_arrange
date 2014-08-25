#pragma once

#include <iostream>
#include <vector>

using std::cout;
using std::cerr;
using std::endl;
using std::vector;

extern void defaultSettings(int &tmp_NUM_CHOICES, vector<int> &scores);

// ざっくりとした上界を計算
extern int getUpperBound(int nd, int np, const vector<int> &scores,
			 const vector<vector<int> > &choices,
			 const vector<int> &capacity);

// ２つのベクトルのユークリッド距離を計算する
extern double computeDistance(const vector<int> &a, const vector<int> &b);

// ベクトルの中身を列挙する
extern void showVector(const vector<int> &v);
