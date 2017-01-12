/*
 * *** Minimum spanning tree for each edge ***
 * Construct a MST. For every edge in the MST, the minimal weight is obviously
 * the weight of the MST. For an edge (u,v) outside the MST, the minimal weight
 * is [weight of MST] + [weight of edge (u,v)] - [heaviest edge on MST path from
 * u to v].
 * The path on the MST from u to v is the path u -> LCA(u,v) -> v.
 * To calculate the LCA, see this article:
 * https://www.topcoder.com/community/data-science/data-science-tutorials/range-minimum-query-and-lowest-common-ancestor/#Another%20easy%20solution%20in%20O(N%20logN,%20O(logN)
 * It's easy to modify that algorithm so that you keep track of the heaviest
 * edge on the path u <--> LCA(u,v) and the path v <--> LCA(u,v).
 */
#include <algorithm>
#include <iostream>
#include <queue>
#include <tuple>
#include <vector>
using namespace std;

typedef long long ll;
typedef vector<ll> vl;
typedef vector<int> vi;
typedef vector<vi> vvi;
typedef pair<int,int> pii;
typedef vector<pii> vpi;
typedef tuple<int,int,int> tiii;
typedef vector<vector<tiii> > vvti;
#define MP make_pair
#define MT make_tuple

struct HeavyQuery {
  int sz;
  int szlog;
  vi depth; // depth of node
  vvi anc; // P[i][j] is 2^j-th ancestor of i
  vvi heavy; // heavy[i][j] is heaviest edge on path from i to 2^j-th ancestor

  HeavyQuery(int n, const vpi &tree_parents) : sz(n), depth(n, 0) {
    szlog = 1;
    while (n) {
      n >>= 1;
      szlog++;
    }
    anc = vvi(sz, vi(szlog, -1));
    heavy = vvi(sz, vi(szlog));

    init(tree_parents);
  }

  void depth_init(int v, const vpi &tree_parents) {
    if (depth[v]) return;
    int parent = tree_parents[v].first;
    if (parent < 0) return;
    depth_init(parent, tree_parents);
    depth[v] = 1 + depth[parent];
  }

  void init(const vpi &tree_parents) {
    for (int i = 0; i < sz; i++) {
      anc[i][0] = tree_parents[i].first;
      heavy[i][0] = tree_parents[i].second;
    }

    for (int j = 1; (1 << j) < sz; j++) {
      for (int i = 0; i < sz; i++) {
        if (anc[i][j-1] >= 0) {
          heavy[i][j] = max(heavy[i][j-1], heavy[anc[i][j-1]][j-1]);
          anc[i][j] = anc[anc[i][j-1]][j-1];
        }
      }
    }

    for (int i = 0; i < sz; i++)
      depth_init(i, tree_parents);
  }

  int heavy_query(int p, int q) {
    int heaviest = 0;

    if (depth[p] < depth[q]) { // WLOG let depth[p] >= depth[q]
      int tmp = p;
      p = q;
      q = tmp;
    }

    int log = 1;
    while ((1 << log) <= depth[p])
      log++;
    log--;

    // find ancestor of p with same depth as q
    for (int i = log; i >= 0; i--) {
      if (depth[p] - (1 << i) >= depth[q]) {
        heaviest = max(heaviest, heavy[p][i]);
        p = anc[p][i];
      }
    }

    if (p == q)
      return heaviest;

    // find LCA(p,q)
    for (int i = log; i >= 0; i--) {
      if (anc[p][i] >= 0 && anc[p][i] != anc[q][i]) {
        heaviest = max(heaviest, max(heavy[p][i], heavy[q][i]));
        p = anc[p][i];
        q = anc[q][i];
      }
    }

    //LCA of p and q is parent[p] now
    heaviest = max(heaviest, max(heavy[p][0], heavy[q][0]));

    return heaviest;
  }
};

int main() {
  ios_base::sync_with_stdio(false); cin.tie(NULL);

  int n, m, u, v, w;
  cin >> n >> m;
  vvti adj_list (n); // (dst, weight, idx)
  for (int i = 0; i < m; i++) {
    cin >> u >> v >> w;
    u--; v--; // make vertices 0-indexed

    adj_list[u].push_back(MT(v,w,i));
    adj_list[v].push_back(MT(u,w,i));
  }

  // construct MST via Prim
  vpi mst_parents(n, MP(-1,0)); // (parent, edge weight)
  vector<bool> visited(n, false);
  ll total_weight = 0;
  priority_queue<tiii> pq; // (-weight, vertex, parent)
  visited[0] = true;
  for (auto it : adj_list[0])
    pq.push(MT(-get<1>(it), get<0>(it), 0));
  while (!pq.empty()) {
    int w = -get<0>(pq.top());
    int u = get<1>(pq.top());
    int p = get<2>(pq.top());
    pq.pop();
    if (visited[u]) continue;

    visited[u] = true;
    mst_parents[u] = MP(p, w);
    total_weight += w;

    for (auto it : adj_list[u]) {
      int v = get<0>(it);
      int neww = get<1>(it);
      if (v != p)
        pq.push(MT(-neww, v, u));
    }
  }

  HeavyQuery solver = HeavyQuery(n, mst_parents);

  vl ans (m);
  for (int u = 0; u < n; u++) {
    for (auto it : adj_list[u]) {
      int v = get<0>(it);
      int w = get<1>(it);
      int idx = get<2>(it);
      if (mst_parents[u].first == v || mst_parents[v].first == u) // edge already in MST
        ans[idx] = total_weight;
      else
        ans[idx] = total_weight + w - solver.heavy_query(u,v);
    }
  }

  for (auto it: ans)
    cout << it << "\n";

  return 0;
}
