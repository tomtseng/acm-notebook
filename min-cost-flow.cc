/* 
 * Min-cost flow stolen from Stanford's ACM notebook. I don't actually know how
 * the implementation works, but it runs some form of successive shortest paths
 * with potentials.
 *
 * To see the actual flow, look at positive values in flow matrix.
 *
 * Should be able to deal with having both an edge (i,j) and it's reverse (j,i).
 * Having multiple edges (i,j), however, is not possible.
 * Negative costs will probably ruin this. Negative cycles are definitely bad.
 *
 * Problem Description
 * -------------------
 * Codeforces 237E: Build String
 * Straightforward min-cost flow. Have 26 nodes represent each of the possible
 * characters. Make each string a node and connect them to the character nodes
 * in the obvious way.
 */
#include <iostream>
#include <limits>
#include <vector>

using namespace std;

typedef vector<int> vi;
typedef vector<vi> vvi;
typedef long long ll;
typedef vector<ll> vl;
typedef vector<vl> vvl;
typedef pair<int, bool> pib;
typedef vector<pib> vpib;

const ll INF = numeric_limits<ll>::max() / 4;

struct MinCostFlow {
  int n;
  vvl capacities,
      flows,
      costs;
  vl dists,
     potentials,
     widths; // widths[i]: amount of flow through a path ending at i
  vpib parents; // (parent, direction)
  vector<bool> visited;

  MinCostFlow(int _n) :
    n(_n), capacities(_n, vl(_n, 0)), flows(_n, vl(_n, 0)), costs(_n, vl(_n)),
    dists(_n), potentials(_n, 0), widths(_n), parents(_n), visited(_n) {}

  void add_edge(int from, int to, ll cap, ll cost) {
    capacities[from][to] = cap;
    costs[from][to] = cost;
  }

  void relax(int s, int k, ll cap, ll cost, bool dir) {
    ll reduced = dists[s] + potentials[s] - potentials[k] + cost;
    if (cap && reduced < dists[k]) {
      dists[k] = reduced;
      parents[k] = make_pair(s, dir);
      widths[k] = min(cap, widths[s]);
    }
  }

  ll shortest_path(int s, int t) {
    fill(visited.begin(), visited.end(), false);
    fill(dists.begin(), dists.end(), INF);
    fill(widths.begin(), widths.end(), 0);
    dists[s] = 0;
    widths[s] = INF;

    while (s != -1) {
      int best = -1;
      visited[s] = true;
      for (int k = 0; k < n; k++) {
        if (visited[k]) continue;
        relax(s, k, capacities[s][k] - flows[s][k], costs[s][k], true);
        relax(s, k, flows[k][s], -costs[k][s], false);
        if (best == -1 || dists[k] < dists[best]) best = k;
      }
      s = best;
    }

    for (int k = 0; k < n; k++)
      potentials[k] = min(potentials[k] + dists[k], INF);

    return widths[t];
  }

  pair<ll, ll> get_flow(int s, int t) {
    ll total_flow = 0,
       total_cost = 0;
    while (ll amnt = shortest_path(s, t)) {
      total_flow += amnt;
      for (int v = t; v != s; v = parents[v].first) {
        if (parents[v].second) {
          flows[parents[v].first][v] += amnt;
          total_cost += amnt * costs[parents[v].first][v];
        } else {
          flows[v][parents[v].first] -= amnt;
          total_cost -= amnt * costs[v][parents[v].first];
        }
      }
    }
    return make_pair(total_flow, total_cost);
  }
};

int main() {
  ios_base::sync_with_stdio(false); cin.tie(NULL);
  int n, tlen;
  string t;
  vector<string> ss (100);
  vi as (100);

  cin >> t >> n;
  tlen = t.size();
  for (int i = 0; i < n; i++)
    cin >> ss[i] >> as[i];

  int sink = 1 + n + 26;
  MinCostFlow solver(sink+1);

  for (int i = 0; i < n; i++)
    solver.add_edge(0, i+1, as[i], i+1); // source -> s

  for (int i = 1, c = 'a'; i <= 26; i++, c++) {
    for (int j = 0; j < n; j++) {
      int scnt = 0;
      int slen = ss[j].length();
      for (int k = 0; k < slen; k++) {
        if (ss[j][k] == c) scnt++;
      }
      solver.add_edge(j+1, sink-i, scnt, 0); // s -> char
    }

    int tcnt = 0;
    for (int k = 0; k < tlen; k++) {
      if (t[k] == c) tcnt++;
    }
    solver.add_edge(sink-i, sink, tcnt, 0); // char -> sink
  }

  pair<ll, ll> ans = solver.get_flow(0, sink);

  cout << (ans.first == tlen ? ans.second : -1) << endl;

  return 0;
}
