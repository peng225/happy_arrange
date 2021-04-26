#include <arg_parser.h>
#include <read_input.h>
#include <common.h>
#include <node.h>
#include <arrange.h>

#include <iostream>

using std::cout;
using std::cerr;
using std::endl;
using std::begin;
using std::end;
using std::string;
using std::stoi;
using std::ifstream;

int main(int argc, char** argv) {
    //コマンドライン引数のチェック
    // 志望数と各志望度のスコアをセット

    int numChoices;
    std::vector<int> scores;
    ArgParser ap;

    ap.genDefaultSettings(numChoices, scores);

    bool verbose = false;
    bool hopelessCut = false;
    char inputFileName[100] = {};
    ap.argParse(argc, argv, verbose, hopelessCut, numChoices, scores, inputFileName);
    const int NUM_CHOICES = numChoices;

    // 必要なデータを読み込む
    ReadInput ri;
    int numPeople, numDept;
    std::vector<int> capacity, choicesID;
    std::vector<std::vector<int> > choices;
    ri.readData(inputFileName, NUM_CHOICES, scores, numPeople, numDept,
                capacity, choices, choicesID);
    const int NUM_PEOPLE = numPeople;
    const int NUM_DEPT = numDept;

    // 部署番号が小さい部署を志望している人を先に処理するようにソート
    sortFollowerWithMaster(begin(choices), end(choices), begin(choicesID),
            end(choicesID));
    std::vector<int> result;
    int score = 0;
    std::list<Node> q;
    Arrange arrange;
    // アルゴリズムの本体
    q = arrange.pdpSearch(scores, capacity, choices, choicesID, verbose, hopelessCut);
    arrange.pdpSelect(q, result, score);
    sortFollowerWithMaster(begin(choicesID), end(choicesID), begin(result),
            end(result));

    // 空行を挿入
    cout << endl;

    cout << "result:" << endl;
    showVector(result);

    int upper = getUpperBound(NUM_DEPT, NUM_PEOPLE, scores, choices, capacity);
    cout << "theoretical upper bound: " << endl << upper << endl;

    // 全体の幸福度を表示
    cout << "total happiness: " << endl << score << endl;

    return 0;
}

