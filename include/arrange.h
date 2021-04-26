#pragma once

#include <node.h>
#include <common.h>

#include <vector>
#include <list>
#include <map>
//#include <set>

class Arrange
{
public:
    // Pseudo Dynamic Programmingの探索フェイズ
    std::list<Node> pdpSearch(const std::vector<int> &scores,
            std::vector<int> &capacity,
            std::vector<std::vector<int> > &choices,
            std::vector<int> &choicesID,
            bool verbose,
            bool hopelessCut);

    // Pseudo Dynamic Programmingの選択フェイズ
    void pdpSelect(std::list<Node> &q,
            std::vector<int> &result,
            int &score);

private:
    const double CUT_OFF_COEF = 1.0;
    //const double CUT_OFF_COEF = 1.6;

    // 望みが薄いノードを切り落とすためのしきい値を計算する
    double getCutOffLowerBound(std::list<Node> &q,
            double scoreMean, double scoreVariance,
            int d, int numPeople);

    // 同じ部署の選択集合を持つもののうち、劣っているノードをすべて削除する
    // void rmInferiorNodes(std::list<Node> &q, const std::map<multiset<int>, int> &score_table);

    // queueに新しい状態を追加する
    void addNewState(int dept, bool verbose,
            const Node &node, const std::vector<int> &target,
            Node &newNode,
            std::map<std::vector<int>, std::list<Node>::iterator> &score_table,
            std::list<Node> &q,
            bool isNew);


    void rmInferiorNodes(std::list<Node> &q, const std::map<std::vector<int>, int> &score_table);
};

