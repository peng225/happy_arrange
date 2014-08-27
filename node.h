#pragma once

#include <vector>
#include <set>

using std::vector;
using std::multiset;

class Node
{
private:
  int depth;
  int score;
  vector<int> history;
  /* multiset<int> depts; */
  vector<int> depts;
public:
 Node(int d, int nd) : depth(d), score(0), history({}), depts({})
    {
      depts.resize(nd);
    }

  void addHistory(int hist)
  {
    history.push_back(hist);
  }

  const vector<int>& getHistory() const
  {
    return history;
  }

  void setScore(int score)
  {
    this->score = score;
  }

  int getScore() const
  {
    return score;
  }
  
  void addDept(int dept)
  {
    /* depts.insert(dept); */
    depts.at(dept)++;
  }
  
  int getDepth() const
  {
    return depth;
  }

  void incrementDepth()
  {
    depth++;
  }
  

  int getNumDept(int dept) const
  {
    /* return depts.count(dept); */
    return depts.at(dept);
  }

  /* const multiset<int>& getDepts() const */
  /* { */
  /*   return depts; */
  /* } */
  const vector<int>& getDepts() const
  {
    return depts;
  }
};
