#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>

using graph_t = std::vector<std::vector<std::uint32_t>>;

void dfs(std::uint32_t from, std::uint32_t &current_component, std::vector<std::uint32_t> &components, const graph_t &graph)
{
    components[from] = current_component;
    for (std::uint32_t to: graph[from])
    {
        if (components[to] == 0)
        {
            dfs(to, current_component, components, graph);
        }
    }
}

int main() 
{
    std::uint32_t n, m;
    std::cin >> n >> m;
    
    graph_t graph(n + 1);
    
    for (std::uint32_t i = 0; i < m; ++i)
    {
        std::uint32_t from, to;
        std::cin >> from >> to;
        graph[from].push_back(to);
        graph[to].push_back(from);
    }

    std::vector<std::uint32_t> components(n + 1);
    std::uint32_t components_count = 0;

    for (std::uint32_t i = 1; i < n + 1; ++i)
    {
        if (components[i] == 0)
        {
            dfs(i, ++components_count, components, graph);
        }
    }

    std::vector<std::vector<std::uint32_t>> result(components_count);
    for (std::uint32_t i = 1; i < components.size(); ++i)
    {
        result[components[i] - 1].push_back(i);
    }

    for (auto &component: result)
    {
        std::sort(component.begin(), component.end());
    }

    std::sort(result.begin(), result.end(), [](const auto &first, const auto &second)
    {
        return first[0] < second[0];
    });

    for (const auto &component: result)
    {
        for (std::size_t i = 0; i < component.size(); ++i)
        {
            std::cout << component[i] << " "; 
        }
        std::cout << std::endl;
    }

    return 0;
}