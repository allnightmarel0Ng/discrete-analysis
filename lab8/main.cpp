#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using graph = std::vector<std::vector<int>>;

std::vector<int> topological_sort(const graph &adj, std::vector<int> &indegree)
{
    std::queue<int> queue;
    for (std::size_t i = 1; i < adj.size(); i++)
    {
        if (indegree[i] == 0)
        {
            queue.push(i);
        }
    }

    std::vector<int> result;

    while (!queue.empty())
    {
        auto node = queue.front();
        queue.pop();
        result.push_back(node);
        for (const auto &neighbor: adj[node])
        {
            indegree[neighbor]--;
            if (indegree[neighbor] == 0)
            {
                queue.push(neighbor);
            }
        }
    }

    return result;
}

int main()
{
    int n, m;
    std::cin >> n >> m;

    graph adj(n + 1);
    std::vector<int> indegree(n + 1, 0);

    for (int i = 0; i < m; ++i) 
    {
        int a, b;
        std::cin >> a >> b;
        adj[a].push_back(b);
        indegree[b]++;
    }

    auto result = topological_sort(adj, indegree);

    if (result.size() != n)
    {
        std::cout << -1 << std::endl;
        return 0;
    }

    for (const auto &node: result)
    {
        std::cout << node << " ";
    }

    std::cout << std::endl;

    return 0;
}