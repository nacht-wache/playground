#include "../GraphInfo.hpp"
#include <gtest/gtest.h>

// Test suite for the Dfs function
TEST(DfsTest, SingleNodeGraph) {
  GraphInfo info;
  info.g.resize(1);
  info.tIn.resize(1);
  info.tOut.resize(1);
  info.color.assign(1, "white");
  info.parent.assign(1, -1);

  Dfs(info, 0);

  ASSERT_EQ(info.timer, 2);
  ASSERT_EQ(info.tIn[0], 0);
  ASSERT_EQ(info.tOut[0], 1);
  ASSERT_EQ(info.parent[0], -1);
  ASSERT_EQ(info.color[0], "black");
}

TEST(DfsTest, SimplePathGraph) {
  GraphInfo info;
  info.g.resize(3);
  info.g[0].push_back(1);
  info.g[1].push_back(2);
  info.tIn.resize(3);
  info.tOut.resize(3);
  info.color.assign(3, "white");
  info.parent.assign(3, -1);

  Dfs(info, 0);

  // Assert on traversal times
  ASSERT_EQ(info.tIn[0], 0);
  ASSERT_EQ(info.tIn[1], 1);
  ASSERT_EQ(info.tIn[2], 2);
  ASSERT_EQ(info.tOut[2], 3);
  ASSERT_EQ(info.tOut[1], 4);
  ASSERT_EQ(info.tOut[0], 5);
  ASSERT_EQ(info.timer, 6);

  // Assert on parent pointers
  ASSERT_EQ(info.parent[0], -1);
  ASSERT_EQ(info.parent[1], 0);
  ASSERT_EQ(info.parent[2], 1);

  // Assert on final colors
  ASSERT_EQ(info.color[0], "black");
  ASSERT_EQ(info.color[1], "black");
  ASSERT_EQ(info.color[2], "black");
}

TEST(DfsTest, BranchingGraph) {
  GraphInfo info;
  info.g.resize(4);
  info.g[0].push_back(1);
  info.g[0].push_back(2);
  info.g[2].push_back(3);
  info.tIn.resize(4);
  info.tOut.resize(4);
  info.color.assign(4, "white");
  info.parent.assign(4, -1);

  Dfs(info, 0);

  // Check timer values for a valid Dfs traversal
  // The exact times for tIn and tOut for nodes 1 and 2 may vary
  // depending on the adjacency list order, but they should form a valid
  // traversal.
  ASSERT_EQ(info.tIn[0], 0);
  ASSERT_EQ(info.tOut[0], 7);
  ASSERT_EQ(info.timer, 8);

  // Check parent pointers
  ASSERT_EQ(info.parent[0], -1);
  ASSERT_EQ(info.parent[1], 0);
  ASSERT_EQ(info.parent[2], 0);
  ASSERT_EQ(info.parent[3], 2);

  // Check final colors
  ASSERT_EQ(info.color[0], "black");
  ASSERT_EQ(info.color[1], "black");
  ASSERT_EQ(info.color[2], "black");
  ASSERT_EQ(info.color[3], "black");
}

TEST(DfsTest, GraphWithCycle) {
  GraphInfo info;
  info.g.resize(3);
  info.g[0].push_back(1);
  info.g[1].push_back(2);
  info.g[2].push_back(0);  // This creates a cycle
  info.tIn.resize(3);
  info.tOut.resize(3);
  info.color.assign(3, "white");
  info.parent.assign(3, -1);

  Dfs(info, 0);

  // Since the code checks for "white" color, it will not enter the cycle.
  // The traversal will be 0 -> 1 -> 2. The edge 2 -> 0 will be skipped.
  ASSERT_EQ(info.tIn[0], 0);
  ASSERT_EQ(info.tIn[1], 1);
  ASSERT_EQ(info.tIn[2], 2);
  ASSERT_EQ(info.tOut[2], 3);
  ASSERT_EQ(info.tOut[1], 4);
  ASSERT_EQ(info.tOut[0], 5);
  ASSERT_EQ(info.timer, 6);

  // Check parent pointers
  ASSERT_EQ(info.parent[0], -1);
  ASSERT_EQ(info.parent[1], 0);
  ASSERT_EQ(info.parent[2], 1);
}

TEST(DfsTest, DisconnectedGraph) {
  GraphInfo info;
  info.g.resize(4);
  info.g[0].push_back(1);  // Component 1: 0 -> 1
  info.g[2].push_back(3);  // Component 2: 2 -> 3
  info.tIn.resize(4);
  info.tOut.resize(4);
  info.color.assign(4, "white");
  info.parent.assign(4, -1);

  // Start Dfs from node 0. Only the first component will be traversed.
  Dfs(info, 0);

  // Assert that the first component was fully traversed.
  ASSERT_EQ(info.color[0], "black");
  ASSERT_EQ(info.color[1], "black");
  ASSERT_EQ(info.tIn[0], 0);
  ASSERT_EQ(info.tIn[1], 1);
  ASSERT_EQ(info.tOut[1], 2);
  ASSERT_EQ(info.tOut[0], 3);

  // Assert that the second component was not visited.
  ASSERT_EQ(info.color[2], "white");
  ASSERT_EQ(info.color[3], "white");
  ASSERT_EQ(info.parent[2], -1);
  ASSERT_EQ(info.parent[3], -1);
}

TEST(DfsTest, LargeComplexGraph) {
  GraphInfo info;
  info.g.resize(1000);
  info.tIn.resize(1000);
  info.tOut.resize(1000);
  info.color.assign(1000, "white");
  info.parent.assign(1000, -1);

  // Build a complex graph structure with branching and cycles
  for (int i = 0; i < 999; ++i) {
    info.g[i].push_back(i + 1);
  }
  info.g[100].push_back(50);   // A back edge to create a cycle
  info.g[200].push_back(300);  // Another branch

  Dfs(info, 0);

  // Verify that all reachable nodes were visited
  ASSERT_EQ(info.color[999], "black");
  ASSERT_EQ(info.timer, 2000);  // 1000 nodes, each with tIn and tOut
}

TEST(DfsTest, DirectedAcyclicGraph) {
  GraphInfo info;
  info.g.resize(5);
  info.g[0].push_back(1);
  info.g[0].push_back(2);
  info.g[1].push_back(3);
  info.g[2].push_back(3);
  info.g[2].push_back(4);
  info.tIn.resize(5);
  info.tOut.resize(5);
  info.color.assign(5, "white");
  info.parent.assign(5, -1);

  Dfs(info, 0);

  // Assert on parent pointers
  ASSERT_EQ(info.parent[0], -1);
  ASSERT_EQ(info.parent[1], 0);
  ASSERT_EQ(info.parent[2], 0);
  ASSERT_EQ(info.parent[3], 1);  // Or 2, depending on traversal order
  ASSERT_EQ(info.parent[4], 2);

  // In a DAG, the tOut values provide a reverse topological sort.
  // The tOut of a node is always greater than the tOut of its children.
  ASSERT_GT(info.tOut[0], info.tOut[1]);
  ASSERT_GT(info.tOut[0], info.tOut[2]);
  ASSERT_GT(info.tOut[1], info.tOut[3]);
  ASSERT_GT(info.tOut[2], info.tOut[3]);
  ASSERT_GT(info.tOut[2], info.tOut[4]);
}

TEST(DfsTest, GraphWithSelfLoopAndMultipleEdges) {
  GraphInfo info;
  info.g.resize(3);
  info.g[0].push_back(0);  // Self-loop
  info.g[0].push_back(1);
  info.g[0].push_back(1);  // Multiple edges
  info.g[1].push_back(2);
  info.tIn.resize(3);
  info.tOut.resize(3);
  info.color.assign(3, "white");
  info.parent.assign(3, -1);

  Dfs(info, 0);

  // A self-loop from a node to itself will be treated as an edge
  // but won't cause infinite recursion if the color check is in place.
  // It's not a tree edge, so it won't change the parent.
  ASSERT_EQ(info.parent[0], -1);
  ASSERT_EQ(info.parent[1], 0);
  ASSERT_EQ(info.parent[2], 1);
  ASSERT_EQ(info.color[0], "black");
  ASSERT_EQ(info.color[1], "black");
  ASSERT_EQ(info.color[2], "black");
}

TEST(DfsTest, DetectBackEdgeAndCycle) {
  GraphInfo info;
  info.g.resize(3);
  info.g[0].push_back(1);
  info.g[1].push_back(2);
  info.g[2].push_back(0);  // Back edge to a node in the current recursion stack
  info.tIn.resize(3);
  info.tOut.resize(3);
  info.color.assign(3, "white");
  info.parent.assign(3, -1);

  // Dfs must be able to handle this. Since node 0 will be "gray" when
  // processing the edge from 2 to 0, this indicates a cycle.
  // The Dfs function itself should handle this without infinite recursion.
  Dfs(info, 0);

  // A simple Dfs() won't detect the cycle explicitly unless modified.
  // However, the test should verify that the traversal terminates correctly
  // without infinite recursion.
  // The parent of 0 should be -1, 1 should be 0, and 2 should be 1.
  ASSERT_EQ(info.parent[0], -1);
  ASSERT_EQ(info.parent[1], 0);
  ASSERT_EQ(info.parent[2], 1);
  ASSERT_EQ(info.color[0], "black");
  ASSERT_EQ(info.color[1], "black");
  ASSERT_EQ(info.color[2], "black");
}
