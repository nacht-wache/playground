#pragma once
#include <vector>
#include <string>

struct GraphInfo {
    std::vector<std::vector<int>> g;
    std::vector<int> tIn, tOut;
    int timer = 0;
    std::vector<std::string> color;
    std::vector<int> parent;
};

inline void Dfs(GraphInfo & info, int v, int p = - 1) {
    info.tIn[v] = info.timer++;
    info.parent[v] = p;
    info.color[v] = "grey";
    for (int to : info.g[v]) {
        if (info.color[to] == "white") {
            Dfs(info, to, v);
        }
    }
    info.tOut[v] = info.timer++;
    info.color[v] = "black";
}
