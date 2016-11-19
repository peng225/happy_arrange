#include "common.h"

void trimAndSplit(string target, string delim, list<string> &list_string) {
	boost::trim(target);
	boost::split(list_string, target, boost::is_any_of(delim),
			boost::algorithm::token_compress_on);
}

void defaultSettings(int &numChoices, vector<int> &scores) {
	numChoices = 3;
	scores.resize(numChoices + 1);
	scores[0] = 5;
	scores[1] = 3;
	scores[2] = 2;
	scores[3] = 0;
}

/*
 スコアの理論的な上界の１つを求める。
 上限ではないので注意。
 */
int getUpperBound(int nd, int np, const vector<int> &scores,
		const vector<vector<int> > &choices, const vector<int> &capacity) {
	int upper = 0;
	// 部署数分ループ
	for (int i = 0; i < nd; i++) {
		int count = 0;
		// 人数分ループ
		for (int j = 0; j < np; j++) {
			if (choices.at(j).at(i) == scores.at(0)) {
				count++;
			}
		}
		// 定員を超える第一志望人員がいたら残念賞
		if (capacity.at(i) < count) {
			upper += scores.at(0) * capacity.at(i);
			upper += scores.at(1) * (count - capacity.at(i));
		} else {
			upper += scores.at(0) * count;
		}
	}
	return upper;
}

double computeDistance(const vector<int> &a, const vector<int> &b) {
	if (a.size() != b.size()) {
		cerr << "Error: Different size vectors are given." << endl;
		exit(1);
	}

	double dist = 0;
	for (int i = 0; i < (int) a.size(); i++) {
		dist += (a.at(i) - b.at(i)) * (a.at(i) - b.at(i));
	}
	dist /= a.size();
	return dist;
}

void showVector(const vector<int> &v) {
	for (vector<int>::const_iterator i = begin(v); i != end(v); i++) {
		cout << *i;
		if (distance(i, end(v)) != 1) {
			cout << ", ";
		} else {
			cout << endl;
		}
	}
}

void openInputFile(char* inputFileName, ifstream &ifs) {
	if (inputFileName[0] == 0) {
		cerr << "Error: You must specify the input file name by -i option."
				<< endl;
		exit(1);
	}

	// ifstream ifs(inputFileName);
	ifs.open(inputFileName);
	if (ifs.fail()) {
		cerr << "Error: Failed to open the file." << endl;
		exit(1);
	}
}

void readData(ifstream &ifs, const int NUM_CHOICES, const vector<int> &scores,
		int &numPeople, int &numDept, vector<int> &capacity,
		vector<vector<int> > &choices, vector<int> &choicesID) {
	string tstr;

	// 人数を読み込む
	const string delim(" \t");
	list<string> list_string;
	getline(ifs, tstr);
	trimAndSplit(tstr, delim, list_string);
	if (list_string.size() != 1) {
		cerr << "Error: Bad data for the number of people." << endl;
		exit(1);
	}
	try {
		numPeople = stoi(tstr);
	} catch (std::invalid_argument &e) {
		cerr << "Error: Failed to convert the input into integer." << endl;
		exit(1);
	}

	if (numPeople <= 0) {
		cerr << "Error: The number of people must be positive." << endl;
		exit(1);
	}

	// 部署数を読み込む
	list_string.clear();
	getline(ifs, tstr);
	trimAndSplit(tstr, delim, list_string);
	if (list_string.size() != 1) {
		cerr << "Error: Bad data for the number of department." << endl;
		exit(1);
	}
	try {
		numDept = stoi(tstr);
	} catch (std::invalid_argument &e) {
		cerr << "Error: Failed to convert the input into integer." << endl;
		exit(1);
	}

	if (numDept <= 0) {
		cerr << "Error: The number of department must be positive." << endl;
		exit(1);
	}

	cout << "# of people:" << endl;
	cout << numPeople << endl;
	cout << "# of department:" << endl;
	cout << numDept << endl;

	// 各部署の定員を読み込む
	list_string.clear();
	capacity.resize(numDept);
	for (vector<int>::iterator i = begin(capacity); i != end(capacity); i++) {
		getline(ifs, tstr);
		trimAndSplit(tstr, delim, list_string);
		if (list_string.size() != 1) {
			cerr << "Error: Bad data for the capacity of each department."
					<< endl;
			exit(1);
		}
		try {
			*i = stoi(tstr);
		} catch (std::invalid_argument &e) {
			cerr << "Error: Failed to convert the input into integer." << endl;
			exit(1);
		}
		if (*i <= 0) {
			cerr << "Error: Capacity of each department must be positive."
					<< endl;
			exit(1);
		}
	}
	cout << "capacity:" << endl;
	showVector(capacity);

	// 部署の定員の総和が人数になっているかチェック
	if (std::accumulate(begin(capacity), end(capacity), 0) != numPeople) {
		cerr
				<< "Error: Summation of capacities of all department must be equal to the number of people."
				<< endl;
		exit(1);
	}

	// 志望度ベクトルの初期化
	choices.resize(numPeople);
	choicesID.resize(numPeople);

	for (vector<vector<int> >::iterator i = begin(choices); i != end(choices);
			i++) {
		i->resize(numDept);
	}

	for (int i = 0; i < numPeople; i++) {
		choicesID.at(i) = i;
	}

	// 志望度ベクトルの値を読み込む
	for (vector<vector<int> >::iterator i = begin(choices); i != end(choices);
			i++) {
		list<string> list_string;
		getline(ifs, tstr);
		trimAndSplit(tstr, delim, list_string);
		if ((int) list_string.size() != NUM_CHOICES) {
			cerr << "Error: The size of score list is wrong." << endl;
			exit(1);
		}

		dynamic_bitset<> isSelected(numDept);

		for (list<string>::iterator j = begin(list_string);
				j != end(list_string); j++) {
			// 各人のj番目の志望を読み込む
			int dept = 0;
			try {
				dept = stoi(*j);
			} catch (std::invalid_argument &e) {
				cerr << "Error: Failed to convert the input into integer."
						<< endl;
				exit(1);
			}
			if (dept < 0 || numDept <= dept) {
				cerr
						<< "Error: The value of dept must be between 0 and '# of dept - 1'."
						<< endl;
				exit(1);
			}
			if (!isSelected.test(dept)) {
				i->at(dept) = scores.at(distance(begin(list_string), j));
				isSelected.set(dept);
			} else {
				cerr << "Error: Multiple choices of one department." << endl;
				exit(1);
			}
		}
	}

	// 全員分の志望度ベクトルを表示
	cout << "choices:" << endl;
	for (vector<vector<int> >::iterator i = begin(choices); i != end(choices);
			i++) {
		showVector(*i);
	}
	cout << endl;
}
