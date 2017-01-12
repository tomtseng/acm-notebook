/*
 * Stolen from Stanford's ACM notebook.
 *
 * Problem Description
 * -------------------
 * Codeforces 375E: Red and Black Tree
 * No one solved this in-contest. But, unlike the contestants, I have the luxury
 * of looking at the editorial and seeing the solution.
 * The solution to this is rather mystical. You turn it into a linear
 * programming problem with
 *
 *   minimize    c[i] * x[i]
 *   subject to  sum over j of D[i][j] * x[j] >= 1          for each i
 *               sum x[i]                     <= num_black
 *               x[i]                         >= 0          for each i
 *   where
 *     c[i]      = 1 if node i is originally red
 *                 0 otherwise
 *     D[i][j]   = 1 if distance from i to j is at most x
 *                 0 otherwise
 *     num_black = number of black nodes in the tree
 *     x[i]      = 1 represents that node i is black in final configuration
 *               = 0 represents that node i is red   in final configuration
 *
 *  After examining at these constraints, it's clear that they must hold in a
 *  solution. What's not clear to me is why the linear programming solution is
 *  always an integer solution. But it works, so there you go.
 */
#include <cmath>
#include <iostream>
#include <limits>
#include <queue>
#include <vector>
using namespace std;

typedef long long ll;
typedef long double ld;
typedef vector<ld> vd;
typedef vector<vd> vvd;
typedef vector<int> vi;

const ld EPS = 1e-9;

/*
 * maximize   c^T x
 * subject to Ax <= b
 *            x> = 0
 *
 *  input: A    m x n matrix
 *         b    m-dim vector
 *         c    n-dim vector
 *         x    vector to store solution
 *
 *  output: value of optimal solution if it exists,
 *          infinity                  if solution is unbounded above,
 *          -infinity                 if infeasible
 *
 *  usage: LPSolver foo(A,b,c); foo.solve(x);
 */
struct LPSolver {
  int m, n;
  vi B, N;
  vvd D;

  LPSolver(const vvd &A, const vd &b, const vd &c) :
    m(b.size()), n(c.size()), B(m), N(n+1), D(m+2, vd(n+2)) {
    for (int i = 0; i < m; i++)
      for (int j = 0; j < n; j++)
        D[i][j] = A[i][j];
    for (int i = 0; i < m; i++) {
      B[i] = n + i;
      D[i][n] = -1;
      D[i][n+1] = b[i];
    }
    for (int j = 0; j < n; j++) {
      N[j] = j;
      D[m][j] = -c[j];
    }
    N[n] = -1;
    D[m+1][n] = 1;
  }

  void pivot(int r, int s) {
    for (int i = 0; i < m + 2; i++)
      if (i != r)
        for (int j = 0; j < n + 2; j++)
          if (j != s)
            D[i][j] -= D[r][j] * D[i][s] / D[r][s];
    for (int j = 0; j < n + 2; j++)
      if (j != s)
        D[r][j] /= D[r][s];
    for (int i = 0; i < m + 2; i++)
      if (i != r)
        D[i][s] /= -D[r][s];
    D[r][s] = 1.0 / D[r][s];
    swap(B[r], N[s]);
  }

  bool simplex(int phase) {
    int x = phase == 1 ? m + 1 : m;
    while (true) {
      int s = -1;
      for (int j = 0; j <= n; j++) {
        if (phase == 2 && N[j] == -1) continue;
        if (s == -1 || D[x][j] < D[x][s] || D[x][j] == D[x][s] && N[j] < N[s])
          s = j;
      }
      if (D[x][s] > -EPS) return true;
      int r = -1;
      for (int i = 0; i < m; i++) {
        if (D[i][s] < EPS) continue;
        if (r == -1 || D[i][n+1] / D[i][s] < D[r][n+1] / D[r][s]
         || (D[i][n+1] / D[i][s]) == (D[r][n+1] / D[r][s]) && B[i] < B[r])
          r = i;
      }
      if (r == -1) return false;
      pivot(r, s);
    }
  }

  ld solve(vd &x) {
    int r = 0;
    for (int i = 1; i < m; i++)
      if (D[i][n+1] < D[r][n+1])
        r = i;
    if (D[r][n + 1] < -EPS) {
      pivot(r, n);
      if (!simplex(1) || D[m+1][n+1] < -EPS) return -numeric_limits<ld>::infinity();
      for (int i = 0; i < m; i++)
        if (B[i] == -1) {
          int s = -1;
          for (int j = 0; j <= n; j++)
            if (s == -1 || D[i][j] < D[i][s] || D[i][j] == D[i][s] && N[j] < N[s])
              s = j;
          pivot(i, s);
        }
    }
    if (!simplex(2)) return numeric_limits<ld>::infinity();

    x = vd(n);
    for (int i = 0; i < m; i++)
      if (B[i] < n)
        x[B[i]] = D[i][n+1];
    return D[m][n+1];
  }
};

int main() {
  ios_base::sync_with_stdio(false); cin.tie(NULL);

  int n, x, u, v, w, t, total_black = 0;
  cin >> n >> x;
  vector<int> cs (n);
  vector<vector<pair<int, ll> > > adj_list (n);
  vector<vector<ll> > dist (n, vector<ll>(n, numeric_limits<ll>::max()/4)); // node, weight

  for (int i = 0; i < n; i++) {
    cin >> t;
    if (t) total_black++;
    cs[i] = !t;
  }
  for (int i = 0; i < n-1; i++) {
    cin >> u >> v >> w;
    u--; v--;
    adj_list[u].emplace_back(v, w);
    adj_list[v].emplace_back(u, w);
  }

  priority_queue<pair<ll,int>> pq; // dist, node
  for (int i = 0; i < n; i++) {
    dist[i][i] = 0;
    pq.emplace(0, i);
    while (!pq.empty()) {
      pair<ll,int> p = pq.top();
      pq.pop();

      for (auto it : adj_list[p.second]) {
        if (dist[i][p.second] + it.second < dist[i][it.first]) {
          dist[i][it.first] = dist[i][p.second] + it.second;
          pq.emplace(-dist[i][it.first], it.first);
        }
      }
    }
  }

  vd B(n+1),
     C(n),
     sol;
  vvd A(n+1, vd(n));
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      A[i][j] = (dist[i][j] <= x) ? -1 : 0;
    }
    B[i] = -1;
    A[n][i] = 1;
  }
  B[n] = total_black;
  for (int i = 0; i < n; i++)
    C[i] = -cs[i];

  LPSolver solver(A, B, C);

  double dans = solver.solve(sol);

  cout << (abs(dans) > 1000 ? -1 : (int)(-dans + 0.5)) << endl;

  return 0;
}
