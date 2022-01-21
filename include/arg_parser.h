#pragma once

#include <string>
#include <vector>
#include <list>

class ArgParser {
public:
    void genDefaultSettings(int &numChoices, std::vector<int> &scores) const;
    void argParse(int argc, char** argv,
                  bool &verbose, bool &hopelessCut, int &numChoices,
                  std::vector<int> &scores, char* inputFileName) const;
private:
    bool isScoresInCorrectFormat(const std::vector<int> &scores) const;
};

