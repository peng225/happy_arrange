#include <read_input.h>
#include <common.h>

#include <boost/dynamic_bitset.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>

#include <iostream>
#include <numeric>


void ReadInput::readData(char* inputFileName, const int NUM_CHOICES,
                         const std::vector<int> &scores,
                         int &numPeople, int &numDept, std::vector<int> &capacity,
                         std::vector<std::vector<int>> &choices, std::vector<int> &choicesID)
{
    openInputFile(inputFileName);

    std::string tstr;

    // 人数を読み込む
    const std::string delim(" \t");
    std::list<std::string> listString;
    getline(ifs, tstr);
    trimAndSplit(tstr, delim, listString);
    if (listString.size() != 1) {
        std::cerr << "Error: Bad data for the number of people." << std::endl;
        exit(1);
    }
    try {
        numPeople = stoi(tstr);
    } catch (std::invalid_argument &e) {
        std::cerr << "Error: Failed to convert the input into integer." << std::endl;
        exit(1);
    }

    if (numPeople <= 0) {
        std::cerr << "Error: The number of people must be positive." << std::endl;
        exit(1);
    }

    // 部署数を読み込む
    listString.clear();
    getline(ifs, tstr);
    trimAndSplit(tstr, delim, listString);
    if (listString.size() != 1) {
        std::cerr << "Error: Bad data for the number of department." << std::endl;
        exit(1);
    }
    try {
        numDept = stoi(tstr);
    } catch (std::invalid_argument &e) {
        std::cerr << "Error: Failed to convert the input into integer." << std::endl;
        exit(1);
    }

    if (numDept <= 0) {
        std::cerr << "Error: The number of department must be positive." << std::endl;
        exit(1);
    }

    std::cout << "# of people:" << std::endl;
    std::cout << numPeople << std::endl;
    std::cout << "# of department:" << std::endl;
    std::cout << numDept << std::endl;

    // 各部署の定員を読み込む
    listString.clear();
    capacity.resize(numDept);
    for(auto &cap : capacity) {
        getline(ifs, tstr);
        trimAndSplit(tstr, delim, listString);
        if (listString.size() != 1) {
            std::cerr << "Error: Bad data for the capacity of each department."
                    << std::endl;
            exit(1);
        }
        try {
            cap = stoi(tstr);
        } catch (std::invalid_argument &e) {
            std::cerr << "Error: Failed to convert the input into integer." << std::endl;
            exit(1);
        }
        if (cap <= 0) {
            std::cerr << "Error: Capacity of each department must be positive."
                    << std::endl;
            exit(1);
        }
    }
    std::cout << "capacity:" << std::endl;
    showVector(capacity);

    // 部署の定員の総和が人数になっているかチェック
    if (std::accumulate(begin(capacity), end(capacity), 0) != numPeople) {
        std::cerr
                << "Error: Summation of capacities of all department must be equal to the number of people."
                << std::endl;
        exit(1);
    }

    // 志望度ベクトルの初期化
    choices.resize(numPeople);
    choicesID.resize(numPeople);

    for(auto &choice : choices) {
        choice.resize(numDept);
    }

    for (int i = 0; i < numPeople; i++) {
        choicesID.at(i) = i;
    }

    // 志望度ベクトルの値を読み込む
    for(auto &choice : choices) {
        std::list<std::string> listString;
        getline(ifs, tstr);
        trimAndSplit(tstr, delim, listString);
        if ((int) listString.size() != NUM_CHOICES) {
            std::cerr << "Error: The size of score list is wrong." << std::endl;
            exit(1);
        }

        boost::dynamic_bitset<> isSelected(numDept);

        for (std::list<std::string>::iterator j = begin(listString);
                j != end(listString); j++) {
            // 各人のj番目の志望を読み込む
            int dept = 0;
            try {
                dept = stoi(*j);
            } catch (std::invalid_argument &e) {
                std::cerr << "Error: Failed to convert the input into integer."
                        << std::endl;
                exit(1);
            }
            if (dept < 0 || numDept <= dept) {
                std::cerr
                        << "Error: The value of dept must be between 0 and '# of dept - 1'."
                        << std::endl;
                exit(1);
            }
            if (!isSelected.test(dept)) {
                choice.at(dept) = scores.at(distance(begin(listString), j));
                isSelected.set(dept);
            } else {
                std::cerr << "Error: Multiple choices of one department." << std::endl;
                exit(1);
            }
        }
    }

    // 全員分の志望度ベクトルを表示
    std::cout << "choices:" << std::endl;
    for(const auto &choice : choices) {
        showVector(choice);
    }
    std::cout << std::endl;
}

void ReadInput::openInputFile(char* inputFileName) {
    if (inputFileName[0] == 0) {
        std::cerr << "Error: You must specify the input file name by -i option."
                << std::endl;
        exit(1);
    }

    // ifstream ifs(inputFileName);
    ifs.open(inputFileName);
    if (ifs.fail()) {
        std::cerr << "Error: Failed to open the file." << std::endl;
        exit(1);
    }
}

void ReadInput::trimAndSplit(std::string target, std::string delim,
                             std::list<std::string> &listString) const
{
    boost::trim(target);
    boost::split(listString, target, boost::is_any_of(delim),
            boost::algorithm::token_compress_on);
}

