// SPOJ - HORRIBLE
// Range increment, range sum segtree
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

const int N = 1 << 17;
const int NN = N * 2;
ll A[NN];
ll lazy[NN];

void push_lazy(int v, int l, int r) {
  ll z;
  if ((z = lazy[v]) != 0) {
    A[v] += z * (r - l + 1);
    if (v < N) {
      lazy[2 * v]     += z;
      lazy[2 * v + 1] += z;
    }
    lazy[v] = 0;
  }
}

ll query(int v, int ql, int qr, int l, int r) {
  push_lazy(v, l, r);
  if (ql == l && qr == r)
    return A[v];

  const int m = (l + r) / 2;
  ll ret = 0;
  if (ql <= m)
    ret += query(2 * v, ql, min(qr, m), l, m);
  if (qr > m)
    ret += query(2 * v + 1, max(m + 1, ql), qr, m + 1, r);
  return ret;
}

void update(int v, int ql, int qr, int l, int r, int x) {
  if (ql == l && qr == r) {
    lazy[v] += x;
    push_lazy(v, l, r);
    return;
  }

  push_lazy(v, l, r);
  A[v] += (ll)x * (qr - ql + 1);

  const int m = (l + r) / 2;
  if (ql <= m)
    update(2 * v, ql, min(qr, m), l, m, x);
  if (qr > m)
    update(2 * v + 1, max(m + 1, ql), qr, m + 1, r, x);
}

void add(int l, int r, int x) {
  update(1, l, r, 0, N - 1, x);
}

ll sum(int l, int r) {
  return query(1, l, r, 0, N - 1);
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
      } else
        cout << sum(p, q) << '\n';
    }

    fill(A, A + NN, 0);
    fill(lazy, lazy + NN, 0);
  }

  return 0;
}
