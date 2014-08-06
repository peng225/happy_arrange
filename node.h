#include <set>

using std::multiset;

class Node
{
private:
  int depth;
  multiset<int> depts;
public:
  Node(int d)
    {
      depth = d;
    }
  
  void addDept(int val)
  {
    depts.insert(val);
  }
  
  int getDepth()
  {
    return depth;
  }

  void incrementDepth()
  {
    depth++;
  }
  

  int getNumDept(int dept)
  {
    return depts.count(dept);
  }

  const multiset<int>& getDepts()
  {
    return depts;
  }
};
