#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <string>

// ざっくりとした上界を計算
extern int getUpperBound(int nd, int np, const std::vector<int> &scores,
		const std::vector<std::vector<int>> &choices, const std::vector<int> &capacity);

// ２つのベクトルのユークリッド距離を計算する
extern double computeDistance(const std::vector<int> &a, const std::vector<int> &b);

// ベクトルの中身を列挙する
extern void showVector(const std::vector<int> &v);

// masterの値を元にfollowerの値をソートする
// アルゴリズムはクイックソートに準ずる
extern void sortFollowerWithMaster(std::vector<int>::iterator m_b,
		std::vector<int>::iterator m_e,
		std::vector<int>::iterator f_b,
		std::vector<int>::iterator f_e);

extern void sortFollowerWithMaster(std::vector<std::vector<int> >::iterator m_b,
		std::vector<std::vector<int> >::iterator m_e,
		std::vector<int>::iterator f_b,
		std::vector<int>::iterator f_e);

