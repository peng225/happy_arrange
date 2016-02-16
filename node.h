#pragma once

#include <vector>
#include <set>
#include <list>

using std::vector;
using std::multiset;
using std::list;

class Node
{
private:
  int score;
  list<int> history;
  /* multiset<int> depts; */
  vector<int> depts;
public:
 Node() : score(0), history({}), depts({})
    {
    }
 Node(int nd) : score(0), history({}), depts({})
    {
      depts.resize(nd);
    }

  void addHistory(int hist)
  {
    history.push_back(hist);
  }

  void delHistory()
  {
    history.pop_back();
  }

  // const vector<int>& getHistory() const
  const list<int>& getHistory() const
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
    depts.at(dept)++;
  }

  void delDept(int dept)
  {
    depts.at(dept)--;
  }

  int getNumDept(int dept) const
  {
    return depts.at(dept);
  }

  const vector<int>& getDepts() const
  {
    return depts;
  }
};
