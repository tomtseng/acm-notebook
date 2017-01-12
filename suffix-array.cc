/*
 * Suffix array computing through prefix-doubling, O(n log^2 n) time. LCP array
 * is computed through Kasai's algorithm, O(n) time.
 *
 * Problem Description
 * -------------------
 * SPOJ: SUBST1
 */
#include <algorithm>
#include <iostream>
using namespace std;
typedef long long ll;

#define MP make_pair

const int MAXN = 50000;

int sa[MAXN + 1];
int sainv[MAXN + 1];
int lcp[MAXN];
pair<pair<int, int>, int> sascr[MAXN + 1]; // scratch

void compute_lcp(const string &str) {
  const int len = str.size();

  int match = 0;
  for (int i = 0; i < len; i++) {
    const int adji = sa[sainv[i] + 1];
    const int slen = len - max(i, adji);
    while (match < slen && str[i + match] == str[adji + match])
      match++;
    lcp[sainv[i]] = match;
    if (match > 0)
      match--;
  }
}

inline void compute_sainv(int len) {
  for (int i = 0; i <= len; i++)
    sainv[sa[i]] = i;
}

void compute_sa(const string& str) {
  const int len = str.size();
  for (int i = 0; i < len; i++)
    sascr[i] = MP(MP(str[i], 0), i);
  sascr[len] = MP(MP(-1, -1), len);
  sort(sascr, sascr + len);
  for (int off = 1; off < len; off <<= 1) {
    int rep = 0;
    for (int i = 0; i < len; i++) {
      const bool diff = sascr[i].first != sascr[i + 1].first;
      sascr[i].first.first = rep;
      rep += diff;

      sa[sascr[i].second] = i;
    }
    for (int i = 0; i < len; i++) {
      const int nidx = sascr[i].second + off;
      sascr[i].first.second = nidx < len ? sascr[sa[nidx]].first.first : -1;
    }
    sort(sascr, sascr + len);
  }
  for (int i = 0; i < len; i++)
    sa[i] = sascr[i].second;
  sa[len] = len;

  compute_sainv(len);
  compute_lcp(str);
}

int main() {
  ios_base::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);
  int T;
  cin >> T;
  while (T--) {
    string s;
    cin >> s;
    compute_sa(s);
    ll ans = (s.size() * (s.size() + 1)) / 2;
    for (int i = 0; i < s.size(); i++)
      ans -= lcp[i];
    cout << ans << '\n';
  }
  return 0;
}
