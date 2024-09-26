#include <iostream>
#include <vector>

struct token_position
{
    int line_number;
    int index_in_line;
};

std::vector<std::string> tokenize(const std::string &pattern)
{
    std::vector<std::string> result;
    std::string word;
    for (size_t i = 0; i <= pattern.size(); ++i) {
        if (i < pattern.size() && std::isalpha(pattern[i]))
        {
            if (word.size() < 16)
            {
                word += std::tolower(pattern[i]);
            }
        }
        else if (!word.empty())
        {
            result.push_back(word);
            word.clear();
        }
    }

    return result;
}

std::vector<int> compute_prefix_function(const std::vector<std::string> &pattern_tokens)
{
    std::vector<int> result(pattern_tokens.size(), 0);

    int k = 0;
    for (size_t i = 1; i < pattern_tokens.size(); ++i)
    {
        while (k > 0 && pattern_tokens[k] != pattern_tokens[i])
        {
            k = result[k - 1];
        }
        if (pattern_tokens[k] == pattern_tokens[i])
        {
            k++;
        }
        result[i] = k;
    }

    return result;
}

void process_line(const std::string &line, int line_number, const std::vector<std::string> &pattern_tokens, const std::vector<int> &prefix_function, int &matched_tokens_count, std::vector<token_position> &position_buffer, int &buffer_index, int pattern_length)
{
    std::string word;
    int index_in_line = 0;
    for (std::size_t i = 0; i <= line.size(); ++i)
    {
        if (i < line.size() && std::isalpha(line[i]))
        {
            word += std::tolower(line[i]);
        }
        else if (!word.empty())
        {
            index_in_line++;
            buffer_index = (buffer_index + 1) % pattern_length;
            position_buffer[buffer_index] = {line_number, index_in_line};

            while (matched_tokens_count > 0 && pattern_tokens[matched_tokens_count] != word)
            {
                matched_tokens_count = prefix_function[matched_tokens_count - 1];
            }

            if (pattern_tokens[matched_tokens_count] == word)
            {
                matched_tokens_count++;
            }
            else
            {
                matched_tokens_count = 0;
            }

            if (matched_tokens_count == pattern_length)
            {
                std::cout << position_buffer[(buffer_index + 1) % pattern_length].line_number << ", " << position_buffer[(buffer_index + 1) % pattern_length].index_in_line << "\n";
                matched_tokens_count = prefix_function[matched_tokens_count - 1];
            }
            word.clear();
        }
    }
}

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string pattern;
    std::getline(std::cin, pattern);

    auto pattern_tokens = tokenize(pattern);
    int pattern_tokens_size = pattern_tokens.size();
    auto prefix_function = compute_prefix_function(pattern_tokens);

    std::vector<token_position> position_buffer(pattern_tokens_size);
    int buffer_index = -1;

    std::string line;
    int line_number = 0;
    int matched_tokens_count = 0;
    while (std::getline(std::cin, line))
    {
        line_number++;
        process_line(line, line_number, pattern_tokens, prefix_function, matched_tokens_count, position_buffer, buffer_index, pattern_tokens_size);
    }

    return 0;
}