#include <iostream>
#include <cmath>
static inline float softmax(float* buffer, int& begin_idx, const int& class_num, int& actual_idx)
{
    float sum_exp = 0;
    for (int i = begin_idx; i < class_num; i++) sum_exp += std::exp(buffer[i]);
    return std::exp(buffer[actual_idx]) / sum_exp;
}
int main()
{
    float buffer[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int begin_idx = 0;
    int class_num = 10;
    int actual_idx = 5;
    softmax(buffer, begin_idx, class_num, actual_idx);
    std::cout << "softmax result: " << softmax(buffer, begin_idx, class_num, actual_idx) << std::endl;
    return 0;

}