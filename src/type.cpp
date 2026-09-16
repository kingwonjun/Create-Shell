//
// Created by app82 on 2026-09-16.
//

#include "type.h"
#include <string>
#include <vector>

using namespace std;

int solution(vector<vector<int>> signals) {
    vector<int> yellow_sum;
    int answer = 0;
    int j = 20;
    for (int i = 0; i < signals.size(); i++) {
        yellow_sum.push_back(signals[i][0] + signals[i][1] + signals[i][2]);
    }
    for (int i = 0; i < signals.size() - 1; i++) {
        while (j-- > 2)
        {
            if (yellow_sum[i] % j == 0 && yellow_sum[i + 1] % j == 0)
            {
                yellow_sum[i + 1] = j;
                break;
            }
        }
    }
    return (answer + 1);
}