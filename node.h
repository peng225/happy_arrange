#include <set>

using std::multiset;

class Node
{
private:
  int depth;
  multiset<int> depts;
public:
  void addDept(int val)
  {
    depts.insert(val);
  }
};
