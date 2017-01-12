#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef pair<int, int> pii;
typedef pair<ll, ll> pll;

#define cin fin
#define cout fout
#define FNAME "curling"
ifstream fin(FNAME ".in");
ofstream fout(FNAME ".out");

#define MP make_pair

const int MAXN = 50000;
pii A[2][MAXN];
pii hull[MAXN + 5];
int hsz; // size of hull

inline ll cross(const pll &A, const pll &B, const pll &C, const pll &D) {
    return (B.first-A.first)*(D.second-C.second) -
           (B.second-A.second)*(D.first-C.first);
}


/*
 * computes convex hull, starting at leftmost point and going clockwise.
 * last point is same as first point.
 * (monotone chain algorithm)
 */
int compute_hull(pii pts[], int psz) {
  hsz = 0;
  sort(pts, pts + psz);

  for (int i = 0; i < psz; i++) {
    while (hsz >= 2 && cross(hull[hsz - 2], hull[hsz - 1], hull[hsz - 1], pts[i]) >= 0)
      hsz--;
    hull[hsz] = pts[i];
    hsz++;
  }

  const int uh = hsz;

  for (int i = psz - 2; i >= 0; i--) {
    while (hsz > uh && cross(hull[hsz - 2], hull[hsz - 1], hull[hsz - 1], pts[i]) >= 0)
      hsz--;
    hull[hsz] = pts[i];
    hsz++;
  }

  return uh;
}

int main() {
  ios_base::sync_with_stdio(false); fin.tie(NULL); fout.tie(NULL);

  int n;
  cin >> n;
  for (int t = 0; t < 2; t++)
    for (int i = 0; i < n; i++) {
      int x, y;
      cin >> x >> y;
      A[t][i] = MP(x, y);
    }

  for (int t = 0; t < 2; t++) {
    const int ai = t, bi = !t;
    int ans = 0;

    const int usz = compute_hull(A[ai], n);
    // (x-coordinate, event type, index)
    // event types:
    //   0 --- begin/update upper hull
    //   1 --- begin/update lower hull
    //   2 --- point from opposing team
    //   3 --- end hull
    vector<tuple<int, int, int>> evs;
    for (int i = 0; i < usz - 1; i++)
      evs.emplace_back(hull[i].first, 0, i);
    evs.emplace_back(hull[usz - 1].first, 3, usz - 1);
    for (int i = usz; i < hsz; i++)
      evs.emplace_back(hull[i].first, 1, i);
    for (int i = 0; i < n; i++)
      evs.emplace_back(A[bi][i].first, 2, i);
    sort(evs.begin(), evs.end());

    int uhidx = -1;
    int lhidx = -1;
    for (auto e : evs) {
      switch (get<1>(e)) {
        case 0:
          uhidx = get<2>(e);
          break;
        case 1:
          lhidx = get<2>(e);
          break;
          case 2:
          if (uhidx >= 0 && lhidx >= 0 &&
              cross(hull[uhidx], hull[uhidx + 1], hull[uhidx + 1], A[bi][get<2>(e)]) <= 0 &&
              cross(hull[lhidx - 1], hull[lhidx], hull[lhidx], A[bi][get<2>(e)]) <= 0 &&
              // dumb special casing for when the hull segment is vertical
              A[bi][get<2>(e)].second <= max(hull[uhidx].second, hull[uhidx + 1].second) &&
              A[bi][get<2>(e)].second >= min(hull[lhidx].second, hull[lhidx - 1].second))
            ans++;
          break;
        case 3:
          uhidx = lhidx = -1;
          break;
      }
    }

    cout << ans << (t == 0 ? ' ' : '\n');
  }
  return 0;
}
