#include <stat.h>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/variance.hpp>

using namespace boost::accumulators;

double Stat::getScoreMean(const std::vector<int> &scores, int numDept) {
    accumulator_set<int, stats<tag::mean> > acc;

    // scoresには希望が通らなかった場合のスコア(0)も含まれるので、それを除く
    for (int i = 0; i < (int) (numDept - scores.size()) + 1; i++) {
        acc(0);
    }

    for(const auto score : scores) {
        acc(score);
    }

    return mean(acc);
}

double Stat::getScoreVariance(const std::vector<int> &scores, int numDept) {
    accumulator_set<int, stats<tag::variance> > acc;

    // scoresには希望が通らなかった場合のスコア(0)も含まれるので、それを除く
    for (int i = 0; i < (int) (numDept - scores.size()) + 1; i++) {
        acc(0);
    }

    for(const auto score : scores) {
        acc(score);
    }

    return variance(acc);
}

