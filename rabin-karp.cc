/*
 * Codeforces 432D: Prefixes and Suffixes
 * You can solve this with string hashing, which I learned from
 * https://www.facebook.com/notes/jonathan-paulson/string-hashing/10151240595637184
 *
 * String hashing allows constant time string equality checking (with a small
 * probability of error).
 * For each index in the string, find the longest substring starting at the
 * index that matches with the prefix of the string using binary search. Keep a
 * counter that counts the number of longest substrings of a particular length.
 * For each 1 <= i <= n, check if the prefix of length i is equal to the suffix
 * of length i. If so, then you can find the number of occurrences of that
 * prefix by summing over the counter for all len such that len >= i. Of course,
 * that's too slow, so compute prefix sums of the counter beforehand.
 */
#include <iostream>
#include <random>
#include <vector>
using namespace std;

#define MAXN 100005
typedef long long ll;

// lots of globals because I hate myself
const ll M = 1000000007; // big prime number
ll H[256]; // hash value of each char
ll pows[MAXN]; // pows[i] stores 2^i mod M
ll powinvs[MAXN]; // powinvs[i] stores 2^(M-1-i) mod M

ll S[MAXN]; // partial string hashes
int match[MAXN];
ll mps[MAXN]; // reverse prefix sum of match; mps[i] sums match[i..MAXN]

// modpow: computes b^e mod m
ll modpow(ll b, ll e, ll m) {
  b %= m;
  ll res = 1;
  while (e > 0) {
    if (e & 1) res = (res * b) % m;
    b = (b * b) % m;
    e >>= 1;
  }
  return res;
}

// sshash: fetches hash of substring from index i to j inclusive
ll sshash(int i, int j) {
  if (j < i) return 0;
  int ival = ( i ? S[i-1] : 0);
  return (((S[j] - ival) * powinvs[i]) % M + M) % M;
}

// sscomp: check if substrings are equivalent
inline bool sscomp (int i, int j, int l, int k) {
  return sshash(i,j) == sshash(l,k);
}

// init: initialize H, pows, powinvs
void init(const string &str) {
  int len = str.size();

  // put random values into H for hashing
  unsigned seed = 40721678;
  mt19937 gen (seed);
  uniform_int_distribution<> dis(1, M);
  for (char c = 'A'; c <= 'Z'; c++)
    H[c] = dis(gen);

  // precompute powers of 2 mod M
  pows[0] = 1;
  for (int i = 1; i < len; i++)
    pows[i] = (pows[i-1] << 1) % M;

  powinvs[len] = modpow(2, M-len-1, M);
  for (int i = len-1; i >= 0; i--)
    powinvs[i] = (powinvs[i+1] << 1) % M;

  // calculate hashes for all prefixes of string
  S[0] = H[str[0]];
  for (int i = 1; i < len; i++)
    S[i] = (S[i-1] + H[str[i]]*pows[i]) % M;
}

int main() {
  ios_base::sync_with_stdio(false); cin.tie(NULL);

  string str;
  cin >> str;
  int len = str.size();

  init(str);

  // at each index, compute longest match with prefix by binary search
  for (int i = 0; i < len; i++) {
    if (str[i] != str[0]) continue; // no match at all

    int l = 0;
    int h = len-i;
    while (l < h) {
      int m = (l+h)/2;
      if (sscomp(0, m, i, i+m)) l = m+1;
      else h = m;
    }
    if (!sscomp(0, l, i, i+l)) l--;

    match[l+1]++;
  }

  for (int i = len+1; i >= 0; i--)
    mps[i] = mps[i+1] + match[i];

  vector<pair<int,int> > ans;
  for (int i = 1; i <= len; i++)
    if (sscomp(0,i-1, len-i, len-1)) ans.push_back(make_pair(i, mps[i]));

  int cnt = ans.size();
  cout << cnt << "\n";
  for (int i = 0; i < cnt; i++)
    cout << ans[i].first << " " << ans[i].second << "\n";

  return 0;
}
