/*
 * BIT supporting range sum and range increment.
 *
 * Problem Description
 * -------------------
 * SPOJ: HORRIBLE
 */
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

const int N = 100003;
ll fen[2][N + 1];

ll fsum(int idx, ll *b) {
  ll sum = 0;
  while (idx > 0) {
    sum += b[idx];
    idx -= (idx & -idx);
  }
  return sum;
}

void finc(int idx, ll v, ll *b) {
  while (idx <= N) {
    b[idx] += v;
    idx += (idx & -idx);
  }
}

inline ll prefsum(int r) {
  return fsum(r, fen[0]) * r + fsum(r, fen[1]);
}

inline ll sum(int l, int r) {
  return prefsum(r) - prefsum(l - 1);
}

void add(int l, int r, int v) {
  finc(l, v, fen[0]);
  finc(r + 1, -v, fen[0]);
  finc(l, (ll)(-l + 1) * v, fen[1]);
  finc(r + 1, (ll)r * v, fen[1]);
}

int main() {
  ios_base::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);
  int T;
  cin >> T;
  while (T--) {
    int n, c;
    cin >> n >> c;
    for (int i = 0; i < c; i++) {
      int type, p, q, v;
      cin >> type >> p >> q;
      if (type == 0) {
        cin >> v;
        add(p, q, v);
      } else {
        cout << sum(p, q) << '\n';
      } 
    }

    fill(&fen[0][0], &fen[0][0] + 2 * (N + 1), 0);
  }

  return 0;
}
