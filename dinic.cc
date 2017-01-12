// SPOJ -- FASTFLOW
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

const int INFCAP = 1000000005;
const int MAXN = 5005;
typedef struct {
  int to;
  int cap;
} edge;

vector<int> ai[MAXN];
vector<edge> el;
int lev[MAXN];
int SRC;
int DST;
vector<int> lai[MAXN];
unsigned ng[MAXN];
int path[MAXN];
int pathsz;

void addedge(int u, int v, int c) {
  const int idx = el.size();
  const edge e1 = {v, c}, e2 = {u, c};
  ai[u].push_back(idx);
  el.push_back(e1);
  ai[v].push_back(idx + 1);
  el.push_back(e2);
}

bool buildlevel() {
  for (int i = 0; i < MAXN; i++)
    lai[i].clear();
  fill(lev, lev + MAXN, 0);
  queue<int> q;
  q.push(SRC);
  lev[SRC] = 1;
  while (!q.empty()) {
    const int v = q.front();
    q.pop();
    if (v == DST)
      return true;
    for (auto idx : ai[v]) {
      const edge&e = el[idx];
      if (e.cap > 0) {
        if (lev[e.to] == 0) {
          lev[e.to] = lev[v] + 1;
          lai[v].push_back(idx);
          q.push(e.to);
        } else if (lev[e.to] == lev[v] + 1) {
          lai[v].push_back(idx);
        }
      }
    }
  }
  return false;
}

int dfs(int v) {
  if (v == DST)
    return INFCAP;
  for (; ng[v] < lai[v].size(); ng[v]++) {
    const edge& e = el[lai[v][ng[v]]];
    if (e.cap > 0) {
      int fl = dfs(e.to);
      if (fl > 0) {
        path[pathsz++] = lai[v][ng[v]];
        return min(fl, e.cap);
      }
    }
  }
  return 0;
}

ll maxflow(int src, int dst) {
  SRC = src, DST = dst;
  ll ret = 0;
  while (buildlevel()) {
    fill(ng, ng + MAXN, 0);
    int fl;
    while ((fl = dfs(SRC)) > 0) {
      ret += fl;
      for (int i = 0; i < pathsz; i++) {
        el[path[i]].cap -= fl;
        el[path[i] ^ 1].cap += fl;
      }
      pathsz = 0;
    }
  }
  return ret;
}

int main() {
  ios_base::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);

  int n, m;
  cin >> n >> m;
  for (int i = 0; i < m; i++) {
    int a, b, c;
    cin >> a >> b >> c;
    addedge(a, b, c);
  }

  cout << maxflow(1, n) << endl;

  return 0;
}
