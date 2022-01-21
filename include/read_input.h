#pragma once

#include <string>
#include <vector>
#include <list>
#include <fstream>

class ReadInput
{
public:

    void readData(char* inputFileName, const int NUM_CHOICES,
                  const std::vector<int> &scores,
		          int &numPeople, int &numDept, std::vector<int> &capacity,
		          std::vector<std::vector<int>> &choices, std::vector<int> &choicesID);
private:
    std::ifstream ifs;
    void openInputFile(char* inputFileName);
    void trimAndSplit(std::string target, std::string delim,
                      std::list<std::string> &listString) const;
};
