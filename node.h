#pragma once

#include <vector>

using std::vector;

class Node {
private:
	int depth;
	int score;
	vector<int> history;
	vector<int> depts;
public:
	Node(int d, int nd) :
			depth(d), score(0), history( { }), depts( { }) {
		depts.resize(nd);
	}
	Node(const Node &node) {
		depth = node.depth;
		score = node.score;
		history = node.history;
		depts = node.depts;
	}
	Node(Node &&node) {
		depth = node.depth;
		score = node.score;
		history = std::move(node.history);
		depts = std::move(node.depts);
	}

	void addHistory(int hist) {
		history.push_back(hist);
	}

	const vector<int>& getHistory() const {
		return history;
	}

	void setScore(int score) {
		this->score = score;
	}

	int getScore() const {
		return score;
	}

	void addDept(int dept) {
		++depts.at(dept);
	}

	int getDepth() const {
		return depth;
	}

	void incrementDepth() {
		++depth;
	}

	int getNumDept(int dept) const {
		return depts.at(dept);
	}

	const vector<int>& getDepts() const {
		return depts;
	}
};
