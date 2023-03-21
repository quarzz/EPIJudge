#include <istream>
#include <string>
#include <map>
#include <set>
#include <vector>

#include "test_framework/generic_test.h"
#include "test_framework/serialization_traits.h"
#include "test_framework/test_failure.h"
#include "test_framework/timed_executor.h"

// #define log(x) std::cout << #x << ": " << (x) << std::endl
#define log(x) nullptr

using std::map;
using std::set;
using std::vector;

enum class Color { kWhite, kBlack };
struct Coordinate {
  bool operator==(const Coordinate& that) const {
    return x == that.x && y == that.y;
  }

  int x = -1, y = -1;
};

static const Coordinate NullCoordinate;

bool operator<(const Coordinate& l, const Coordinate& r) {
  return l.x < r.x || l.x == r.x && l.y < r.y;
}

namespace {
  using Graph = map<Coordinate, set<Coordinate>>;

  const Graph build_graph(const vector<vector<Color>>& maze) {
    Graph graph;

    const auto insert_if = [&graph, &maze] (Coordinate s, Coordinate u) {
      if (u.x < 0 || u.x >= maze.size())
        return;
      if (u.y < 0 || u.y >= maze.front().size())
        return;
      if (maze[u.x][u.y] == Color::kWhite)
        graph[s].insert(u);
    };
    const auto add_neighbors = [insert_if](int i, int j) {
      insert_if({i, j}, {i - 1, j});
      insert_if({i, j}, {i, j - 1});
      insert_if({i, j}, {i + 1, j});
      insert_if({i, j}, {i, j + 1});
    };

    for (size_t i = 0; i < maze.size(); ++i)
      for (size_t j = 0; j < maze[i].size(); ++j)
        if (maze[i][j] == Color::kWhite)
          add_neighbors(i, j);

    return graph;
  }

  vector<Coordinate> find_path(
    Graph& graph,
    const Coordinate& from,
    const Coordinate& to
  ) {
    std::queue<Coordinate> q;
    q.push(from);
    std::map<Coordinate, Coordinate> visited_from;

    while (!q.empty()) {
      const auto cur = q.front(); q.pop();

      for (const auto& neighbor: graph[cur]) {
        if (visited_from[neighbor] == NullCoordinate) {
          q.push(neighbor);
          visited_from[neighbor] = cur;
        }
      }
    }

    if (visited_from[to] == NullCoordinate) return {};

    vector<Coordinate> path;
    auto cur = to;

    while (!(cur == from)) {
      path.push_back(cur);
      cur = visited_from[cur];
    }

    path.push_back(from);
    std::reverse(path.begin(), path.end());

    return path;
  }
}

vector<Coordinate> SearchMaze(vector<vector<Color>> maze, const Coordinate& s,
                              const Coordinate& e) {
  auto graph = build_graph(maze);
  log("Graph built");
  auto path = find_path(graph, s, e);
  log("Path found");

  return path;
}

namespace test_framework {
template <>
struct SerializationTrait<Color> : SerializationTrait<int> {
  using serialization_type = Color;

  static serialization_type Parse(const json& json_object) {
    return static_cast<serialization_type>(
        SerializationTrait<int>::Parse(json_object));
  }
};
}  // namespace test_framework

namespace test_framework {
template <>
struct SerializationTrait<Coordinate> : UserSerTrait<Coordinate, int, int> {
  static std::vector<std::string> GetMetricNames(const std::string& arg_name) {
    return {};
  }

  static std::vector<int> GetMetrics(const Coordinate& x) { return {}; }
};
}  // namespace test_framework

bool PathElementIsFeasible(const vector<vector<Color>>& maze,
                           const Coordinate& prev, const Coordinate& cur) {
  if (!(0 <= cur.x && cur.x < maze.size() && 0 <= cur.y &&
        cur.y < maze[cur.x].size() && maze[cur.x][cur.y] == Color::kWhite)) {
    return false;
  }
  return cur == Coordinate{prev.x + 1, prev.y} ||
         cur == Coordinate{prev.x - 1, prev.y} ||
         cur == Coordinate{prev.x, prev.y + 1} ||
         cur == Coordinate{prev.x, prev.y - 1};
}

bool SearchMazeWrapper(TimedExecutor& executor,
                       const vector<vector<Color>>& maze, const Coordinate& s,
                       const Coordinate& e) {
  vector<vector<Color>> copy = maze;

  auto path = executor.Run([&] { return SearchMaze(copy, s, e); });

  if (path.empty()) {
    return s == e;
  }

  if (!(path.front() == s) || !(path.back() == e)) {
    throw TestFailure("Path doesn't lay between start and end points");
  }

  for (size_t i = 1; i < path.size(); i++) {
    if (!PathElementIsFeasible(maze, path[i - 1], path[i])) {
      throw TestFailure("Path contains invalid segments");
    }
  }

  return true;
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"executor", "maze", "s", "e"};
  return GenericTestMain(args, "search_maze.cc", "search_maze.tsv",
                         &SearchMazeWrapper, DefaultComparator{}, param_names);
}
