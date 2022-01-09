#include "common.h"

/*
 スコアの理論的な上界の１つを求める。
 上限ではないので注意。
 */
int getUpperBound(int nd, int np, const std::vector<int> &scores,
        const std::vector<std::vector<int> > &choices, const std::vector<int> &capacity) {
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
        /*
          定員を超える第一志望人員がいた場合、
          定員分は第0志望のスコア、
          溢れた人は第1志望のスコアとする。
          実際には溢れた人が第3志望以降になることもあるため、
          これは合計スコアが増える方向に倒していることになる。
        */
        if (capacity.at(i) < count) {
            upper += scores.at(0) * capacity.at(i);
            upper += scores.at(1) * (count - capacity.at(i));
        } else {
            upper += scores.at(0) * count;
        }
    }
    return upper;
}

double computeDistance(const std::vector<int> &a, const std::vector<int> &b) {
    if (a.size() != b.size()) {
        std::cerr << "Error: Different size std::vectors are given." << std::endl;
        exit(1);
    }

    double dist = 0;
    for (int i = 0; i < (int) a.size(); i++) {
        dist += (a.at(i) - b.at(i)) * (a.at(i) - b.at(i));
    }
    dist /= a.size();
    return dist;
}

void showVector(const std::vector<int> &v) {
    for (auto i = cbegin(v); i != cend(v); i++) {
        std::cout << *i;
        if (distance(i, end(v)) != 1) {
            std::cout << ", ";
        } else {
            std::cout << std::endl;
        }
    }
}

void sortFollowerWithMaster(std::vector<int>::iterator m_b,
        std::vector<int>::iterator m_e, std::vector<int>::iterator f_b,
        std::vector<int>::iterator f_e) {
    // masterとfollowerのサイズが一致するかチェック
    if (distance(m_b, m_e) != distance(f_b, f_e)) {
        std::cerr << "Error: Length of master and follower must be the same."
                << std::endl;
        exit(1);
    }

    // 再帰の終了条件
    if (distance(m_b, m_e) <= 1) {
        return;
    }

    std::vector<int>::iterator m_l = m_b;
    std::vector<int>::iterator m_r = m_e - 1;
    std::vector<int>::iterator f_l = f_b;
    std::vector<int>::iterator f_r = f_e - 1;
    int pivot = *m_r;

    /*
     処理の本体。
     masterとfollowerを一緒に更新していくので、
     インクリメントやディクリメントの処理がペアになっている。
     */
    while (distance(m_l, m_r) > 0) {
        while (distance(m_l, m_r) > 0 && *m_l < pivot) {
            ++m_l;
            ++f_l;
        }
        if (distance(m_l, m_r) == 0) {
            break;
        }

        while (distance(m_l, m_r) > 0 && pivot <= *m_r) {
            --m_r;
            --f_r;
        }
        if (distance(m_l, m_r) == 0) {
            break;
        }

        iter_swap(m_l, m_r);
        iter_swap(f_l, f_r);
    }

    iter_swap(m_l, m_e - 1);
    iter_swap(f_l, f_e - 1);

    // 再帰
    sortFollowerWithMaster(m_b, m_l, f_b, f_l);
    sortFollowerWithMaster(m_l + 1, m_e, f_l + 1, f_e);
}

void sortFollowerWithMaster(std::vector<std::vector<int> >::iterator m_b,
        std::vector<std::vector<int> >::iterator m_e, std::vector<int>::iterator f_b,
        std::vector<int>::iterator f_e) {
    if (distance(m_b, m_e) != distance(f_b, f_e)) {
        std::cerr << "Error: Length of master and follower must be the same."
                << std::endl;
        exit(1);
    }

    if (distance(m_b, m_e) <= 1) {
        return;
    }

    std::vector<std::vector<int>>::iterator m_l = m_b;
    std::vector<std::vector<int>>::iterator m_r = m_e - 1;
    std::vector<int>::iterator f_l = f_b;
    std::vector<int>::iterator f_r = f_e - 1;
    std::vector<int> pivot = *m_r;

    while (distance(m_l, m_r) > 0) {
        while (distance(m_l, m_r) > 0 && *m_l < pivot) {
            ++m_l;
            ++f_l;
        }
        if (distance(m_l, m_r) == 0) {
            break;
        }

        while (distance(m_l, m_r) > 0 && pivot <= *m_r) {
            --m_r;
            --f_r;
        }
        if (distance(m_l, m_r) == 0) {
            break;
        }

        iter_swap(m_l, m_r);
        iter_swap(f_l, f_r);
    }

    iter_swap(m_l, m_e - 1);
    iter_swap(f_l, f_e - 1);

    sortFollowerWithMaster(m_b, m_l, f_b, f_l);
    sortFollowerWithMaster(m_l + 1, m_e, f_l + 1, f_e);
}

