#pragma once

#include <vector>

class Stat
{
public:
    /*
      scoresの平均を求める。
      希望から外れた場合というのは潜在的には
      NUM_DEPT - score.size() + 1個存在するので、
      そのあたりも加味する。
     */
    double getScoreMean(const std::vector<int> &scores, int numDept);

    /*
      scoresの分散を求める。
      希望から外れた場合というのは潜在的には
      NUM_DEPT - score.size() + 1個存在するので、
      そのあたりも加味する。
     */
    double getScoreVariance(const std::vector<int> &scores, int numDept);
};

