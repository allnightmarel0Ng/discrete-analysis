#include <iostream>
#include <vector>
#include <string>
#include <cstdint>

std::pair<int, std::uint64_t> get_value_operation_pair(
    std::uint64_t minus_one, 
    std::uint64_t div_by_two, 
    std::uint64_t div_by_three)
{
    if (minus_one <= std::min(div_by_two, div_by_three))
    {
        return std::make_pair(1, minus_one);
    }
    else if (div_by_two <= std::min(minus_one, div_by_three))
    {
        return std::make_pair(2, div_by_two);
    }
    return std::make_pair(3, div_by_three);
}

int main()
{
    int input;
    std::cin >> input;
    std::vector<std::uint64_t> dp(input + 1, 0);
    std::vector<int> prev(input + 1, -1);

    dp[1] = 0;

    for (std::size_t i = 2; i <= input; i++)
    {
        auto pair = get_value_operation_pair(
            dp[i - 1], 
            i % 2 == 0 ? dp[i / 2] : UINT64_MAX, 
            i % 3 == 0 ? dp[i / 3] : UINT64_MAX);

        prev[i] = pair.first;
        dp[i] = i + pair.second;
    }

    std::cout << dp[input] << std::endl;

    while (input != 1) 
    {
        switch (prev[input])
        {
        case 1:
            std::cout << "-1";
            input--;
            break;
        case 2:
            std::cout << "/2";
            input /= 2;
            break;
        case 3:
            std::cout << "/3";
            input /= 3;
            break;
        }

        std::cout << (input != 1 ? ' ' : '\n');
    }

    return 0;
}