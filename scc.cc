/*
 * Codeforces 427E: Checkposts
 * Just find the minimum cost point in each of the strongly connected
 * components.
 */
#include <iostream>
#include <vector>
using namespace std;

typedef vector<int> vi;
typedef vector<vi> vvi;

/*
 * Strongly connected components using Kosaraju's algorithm.
 * Vertices are 0-indexed.
 */
class SCC{
  public:
    SCC(int _n) : n(_n), outs(_n), ins(_n), assignment(_n, -1), visited(_n, false) {}

    void add_edge(int u, int v) {
      outs[u].push_back(v);
      ins[v].push_back(u);
    }

    // Returns vector A where A[i] = root of SCC of i
    vector<int> get_scc() {
      for (int u = 0; u < n; u++)
        visit(u);
      for (auto it = seen.rbegin(); it != seen.rend(); ++it)
        assign(*it, *it);
      return assignment;
    }

  private:
    int n;
    vvi outs, // out-neighbors
        ins;  // in-neighbors
    vi seen,
       assignment; // assignment of SCCs
    vector<bool> visited;

    void visit(int u) {
      if (!visited[u]) {
        visited[u] = true;
        for (auto it : outs[u])
          visit(it);
        seen.push_back(u);
      }
    }

    void assign(int u, int root) {
      if (assignment[u] < 0) {
        assignment[u] = root;
        for (auto it: ins[u])
          assign(it, root);
      }
    }
};

int main() {
  ios_base::sync_with_stdio(false); cin.tie(NULL);

  int n, m, u, v;
  cin >> n;
  vi costs(n);
  for (int i = 0; i < n; i++)
    cin >> costs[i];

  SCC scc_solve(n);

  cin >> m;
  for (int i = 0; i < m; i++) {
    cin >> u >> v;
    scc_solve.add_edge(u-1, v-1);
  }

  vi scc_assign = scc_solve.get_scc();

  vector<int> scc_cost(n, -1);
  vector<int> scc_min_num(n);

  for (int i = 0; i < n; i++) {
    int scc = scc_assign[i];
    if (scc_cost[scc] == -1 || costs[i] < scc_cost[scc]) {
      scc_cost[scc] = costs[i];
      scc_min_num[scc] = 1;
    } else if (costs[i] == scc_cost[scc])
      scc_min_num[scc]++;
  }

  long long min_cost = 0;
  long long ways = 1;

  for (int i = 0; i < n; i++) {
    if (scc_cost[i] >= 0) {
      min_cost += scc_cost[i];
      ways = (ways * scc_min_num[i]) % 1000000007;
    }
  }

  cout << min_cost << " " << ways << endl;

  return 0;
}
