#include <arg_parser.h>
#include <boost/algorithm/string.hpp>
#include <unistd.h>
#include <cstring>
#include <iostream>

void ArgParser::genDefaultSettings(int &numChoices, std::vector<int> &scores) const
{
    numChoices = 3;
    scores.resize(numChoices + 1);
    scores[0] = 5;
    scores[1] = 3;
    scores[2] = 2;
    scores[3] = 0;
}

void ArgParser::argParse(int argc, char** argv,
                           bool &verbose, bool &hopelessCut, int &numChoices,
                           std::vector<int> &scores, char* inputFileName) const
{
    int opt;
    // getoptの返り値は見付けたオプション
    while ((opt = getopt(argc, argv, "hvci:s:")) != -1) {
        switch (opt) {
        // 値をとらないオプション
        case 'h':
            std::cout << "Have a nice day!" << std::endl;
            exit(0);
        case 'v':
            verbose = true;
            break;
        case 'c':
            // 入力ファイル名の設定
            hopelessCut = true;
            break;

            // 値を取る引数の場合は外部変数optargにその値を格納する
        case 'i':
            // 入力ファイル名の設定
            strcpy(inputFileName, optarg);
            break;
        case 's':
            // 各志望のスコアを設定
        {
            const std::string delim(":");
            std::list<std::string> listString;
            scores.resize(0);
            boost::split(listString, optarg, boost::is_any_of(delim));
            try {
                for(const auto &s : listString){
                    scores.push_back(stoi(s));
                }
            } catch(std::invalid_argument &e) {
                std::cerr << "Error: Failed to convert the command line parameter into integer." << std::endl;
                exit(1);
            }

            if(!isScoresInCorrectFormat(scores)) {
                std::cerr << "Invalid score format." << std::endl;
                exit(1);
            }

            numChoices = scores.size();

            // 希望から外れた場合のスコアは問答無用で0
            scores.push_back(0);

            break;
        }

            /* 以下二つのcaseは意味がないようだ.
             getoptが直接エラーを出力してくれるから.
             プログラムを終了するなら意味があるかも知れない */
        case ':':
            /* 値を取る引数に値がなかった場合:を返す. */
            exit(1);

            /* getoptの引数で指定されなかったオプションを受け取ると?を返す. */
        case '?':
            exit(1);
        }
    }
}

bool ArgParser::isScoresInCorrectFormat(const std::vector<int> &scores) const
{
    // スコアがすべて正であるかチェック
    for(auto num : scores){
        if(num <= 0) {
            std::cerr << "Error: Scores must be a positive number." << std::endl;
            return false;
        }
    }

    // スコアの間の順序が正しいかチェック
    for (int i = 0; i < (int) scores.size() - 1; i++) {
        if (scores.at(i) <= scores.at(i + 1)) {
            std::cerr << "Error: Wrong order between scores." << std::endl;
            return false;
        }
    }

    return true;
}

