#include <arrange.h>
#include <stat.h>
#include <common.h>

#include <boost/algorithm/string.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/variance.hpp>

#include <iostream>
#include <algorithm>

using namespace boost::accumulators;

double Arrange::getCutOffLowerBound(std::list<Node> &q, double scoreMean,
        double scoreVariance, int d, int numPeople) {
    assert(numPeople != 1);

    accumulator_set<int, stats<tag::variance, tag::mean> > acc;
    for(const auto& n: q)
    {
        acc(n.getScore());
    }
    double mn = mean(acc);
    double va = variance(acc);
    // double specMn = mn + scoreMean;
    double specSd = sqrt(va + scoreVariance);

    // return specMn - 2.0 * specSd / (d + 1);
    return mn - CUT_OFF_COEF * specSd / (d + 1);
}

void Arrange::rmInferiorNodes(std::list<Node> &q, const std::map<std::vector<int>, int> &score_table) {
    assert(!q.empty());
    for (std::list<Node>::iterator i = begin(q); i != end(q);) {
        if (score_table.find(i->getDepts()) != end(score_table)
                && i->getScore() < score_table.at(i->getDepts())) {
            i = q.erase(i);
            continue;
        }
        ++i;
    }
}

std::list<Node> Arrange::pdpSearch(const std::vector<int> &scores, std::vector<int> &capacity,
        std::vector<std::vector<int> > &choices, std::vector<int> &choicesID, bool verbose,
        bool hopelessCut) {
    // 幅優先探索に使用するqueue
    std::list<Node> q;
    // 枝狩り幅優先探索のためのstd::listに空のルートノード(深さ0)を入れる
    q.emplace_back(0, choices.front().size());

    // これまでに選択した部署の集合をキーとし、スコアをバリューとしたマップ
    std::map<std::vector<int>, std::list<Node>::iterator> score_table;
    Stat st;
    double scoreMean = st.getScoreMean(scores, choices.front().size());
    double scoreVariance = st.getScoreVariance(scores, choices.front().size());

    // 探索深さdが人数より小さい間
    for (int d = 0; d < (int) choices.size(); d++) {
        if (verbose) {
            std::cout << std::endl;
            std::cout << "depth: " << d << std::endl;
            std::cout << "queue size: " << q.size() << std::endl;
        }

        // d人目の志望度ベクトル
        std::vector<int> target = choices.at(d);

        if (verbose) {
            std::cout << choicesID.at(d) << ": ";
            showVector(target);
        }

        // 実際の処理に入る前に劣っているノードを削除する
//        rmInferiorNodes(q, score_table);

        /*
         カットオフ値をここで決めておく。
         この値は現在のqの内容から推測する。
         */
        double cutOff = 0;
        if (hopelessCut) {
            cutOff = getCutOffLowerBound(q, scoreMean, scoreVariance, d,
                    choices.size());
            if (verbose) {
                std::cout << "cutOff: " << cutOff << std::endl;
            }
        }

        int numRemoved = 0;

        assert(!q.empty());
        std::cout << "depth, queue size: " << d << ", " << q.size() << std::endl;

        // 先頭要素の深さがdである間
        while (q.front().getDepth() == d) {
            assert(!q.empty());
            // queueの先頭要素を取り出す
            Node* node = &q.front();
            for (int i = 0; i < (int) choices.front().size(); i++) {
                // 部署iがすでに定員に達していたら
                if (node->getNumDept(i) == capacity.at(i)) {
                    if (verbose) {
                        std::cout << "Dept " << i
                                << " has already reached the limit. Skip!"
                                << std::endl;
                    }
                    continue;
                }

                // 枝刈り閾値よりスコアが小さければスキップ
                if (hopelessCut
                        && cutOff
                                > (node->getDepth() == 0 ?
                                        target.at(i) :
                                        score_table.at(node->getDepts())->getScore()
                                                + target.at(i))) {
                    numRemoved++;
                    if (verbose) {
                        std::cout
                                << "The score is lower than the cut off value. Skip!"
                                << std::endl;
                    }
                    continue;
                }

                // 取り出したノードよりも深さを１つ増やした新規ノード情報を作成
                Node newNode = *node;
                newNode.incrementDepth();
                // 現在注目している人がi番目の部署を選んだという情報を新規ノードに加える
                newNode.addDept(i);

                // もしこれまでに選択された部署の集合が未登録なら
                if (score_table.find(newNode.getDepts()) == end(score_table)) {
                    addNewState(i, verbose, *node, target, newNode, score_table,
                            q, true);
                }
                // 既存のものよりスコアが高ければ
                else if (score_table.at(newNode.getDepts())->getScore()
                        < score_table.at(node->getDepts())->getScore() + target.at(i)) {
                    q.erase(score_table.at(newNode.getDepts()));
                    addNewState(i, verbose, *node, target, newNode, score_table,
                            q, false);
                }
            }
            q.pop_front();
        }

        if (hopelessCut) {
            std::cout << "# of removed node: " << numRemoved << std::endl;
        }
    }
    std::cout << "final queue size: " << q.size() << std::endl;
    return q;
}

void Arrange::pdpSelect(std::list<Node> &q, std::vector<int> &result, int &score) {
    assert(!q.empty());
    score = 0;
    while (!q.empty()) {
        Node node = q.front();
        q.erase(begin(q));
        if (score < node.getScore()) {
            score = node.getScore();
            result = node.getHistory();
        }
    }
}

void Arrange::addNewState(int dept, bool verbose, const Node &node,
        const std::vector<int> &target, Node &newNode,
        std::map<std::vector<int>, std::list<Node>::iterator> &score_table, std::list<Node> &q, bool isNew) {
    // 履歴を更新
    newNode.addHistory(dept);

    if (verbose) {
        if (isNew) {
            std::cout << "new: ";
        } else {
            std::cout << "update: ";
        }
        showVector(newNode.getHistory());
    }
    // 自分が持つ部署集合のスコアを記録
    // 一人目の場合は単純にスコアを記録する
    if (node.getDepth() == 0) {
        newNode.setScore(target.at(dept));
    }
    // 二人目以降は現在のスコアテーブルの値に今回のスコアを足して、新たな状態を記録する
    else {
        newNode.setScore(score_table.at(node.getDepts())->getScore()
                            + target.at(dept));
    }

    if (verbose) {
        if (isNew) {
            std::cout << "new score: " << newNode.getScore() << std::endl;
        } else {
            std::cout << "updated score: " << newNode.getScore() << std::endl;
        }
    }

    // 新規ノードをstd::listに入れる
//    q.push_back(std::move(newNode));
    q.emplace_back(newNode);
    auto itr = end(q);
    --itr;
    score_table[newNode.getDepts()] = itr;
}

