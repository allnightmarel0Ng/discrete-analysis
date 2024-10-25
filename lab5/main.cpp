#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>

class suffix_tree
{
private:
    struct node
    {
        int left, right, appear;
        std::unordered_map<char, int> children;

        node(int left = -1, int right = -1, int appear = -1): left(left), right(right), appear(appear) {}
    };

    std::vector<node> _nodes;
    std::string _text;

    void insert(int left, int right)
    {
        int current = 0;
        int old_left = left;
        while (left <= right)
        {
            if (current == 0)
            {
                if (_nodes[current].children.find(_text[left]) != _nodes[current].children.cend())
                {
                    current = _nodes[current].children[_text[left]];
                }
                if (current == 0)
                {
                    _nodes.emplace_back(left, right, old_left);
                    _nodes[current].children[_text[left]] = _nodes.size() - 1;
                    break; 
                }
            }

            auto start = _nodes[current].left;
            auto finish = _nodes[current].right;
            bool cut = false;

            for (int i = start; i <= finish && left + i - start <= right; ++i)
            {
                if (_text[i] == _text[left + i - start])
                {
                    continue;
                }

                _nodes[current].right = i - 1;
                int old_appear = _nodes[current].appear;
                _nodes[current].appear = -1;

                if (_text[finish] == '$')
                {
                    _nodes.emplace_back(i, finish, old_appear);
                }
                else
                {
                    _nodes.emplace_back(i, finish);
                }

                _nodes.back().children = _nodes[current].children;
                _nodes[current].children.clear();
                _nodes[current].children[_text[i]] = _nodes.size() - 1;
                
                _nodes.emplace_back(left + i - start, right, old_left);
                _nodes[current].children[_text[left + i - start]] = _nodes.size() - 1;
                cut = true;
                break;
            }

            if (cut)
            {
                break;
            }

            int new_left = left + finish - start + 1;
            if (_nodes[current].children.find(_text[new_left]) == _nodes[current].children.cend())
            {
                _nodes.emplace_back(new_left, right, old_left);
                _nodes[current].children[_text[new_left]] = _nodes.size() - 1;
                break;
            }

            current = _nodes[current].children[_text[new_left]];
            left = new_left;
        }
    }

    void dfs(std::vector<int> &appearances, int current)
    {
        if (_nodes[current].children.empty())
        {
            appearances.push_back(_nodes[current].appear);
            return;
        }

        for (const auto &[symbol, child]: _nodes[current].children)
        {
            dfs(appearances, child);
        }
    }

public:

    explicit suffix_tree(const std::string &text):
        _text(text + '$')
    {
        _nodes.emplace_back();
        for (std::size_t i = 0; i < _text.size(); ++i)
        {
            insert(i, _text.size() - 1);
        }
    }

    std::vector<int> search(const std::string &pattern)
    {
        if (pattern.size() == 0)
        {
            return {};
        }

        std::vector<int> result;
        int node = 0, pattern_index = 0, size = pattern.size() - 1;
        bool unequal = false;
        while (pattern_index <= size)
        {
            if (node == 0)
            {
                if (_nodes[node].children.find(pattern[pattern_index]) == _nodes[node].children.cend())
                {
                    break;
                }
                node = _nodes[node].children[pattern[pattern_index]];
            }

            for (int i = 0; _nodes[node].left + i <= _nodes[node].right && i + pattern_index <= size; ++i)
            {
                if (pattern[pattern_index + i] == _text[_nodes[node].left + i])
                {
                    continue;
                }
                unequal = true;
                break;
            }

            if (unequal)
            {
                break;
            }

            pattern_index += _nodes[node].right - _nodes[node].left + 1;
            if (pattern_index > size || _nodes[node].children.find(pattern[pattern_index]) == _nodes[node].children.cend())
            {
                break;
            }

            node = _nodes[node].children[pattern[pattern_index]];
        }

        if (pattern_index > size && !unequal)
        {
            dfs(result, node);
        }
        return result;
    }
};

int main()
{
    std::string text;
    std::getline(std::cin, text);

    suffix_tree tree(text);

    std::string pattern;
    int lines_count = 1;
    while (std::getline(std::cin, pattern))
    {
        auto indices = tree.search(pattern);
        if (pattern.empty() || indices.empty())
        {
            lines_count++;
            continue;
        }
        std::sort(indices.begin(), indices.end());
        std::cout << lines_count << ": ";
        for (std::size_t i = 0; i < indices.size(); i++)
        {
            std::cout << indices[i] + 1;
            if (i != indices.size() - 1)
            {
                std::cout << ", ";
            }
        }
        std::cout << std::endl;
        lines_count++;
    }

    return 0;
}