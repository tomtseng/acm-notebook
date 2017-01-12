/*
 * Codeforces 471D: MUH and Cube Walls 
 * Take the consecutive differences in each wall. Then run your favorite
 * substring finding algorithm (e.g. KMP) on wall differences.
 */
#include <iostream>
#include <vector>
using namespace std;

typedef vector<int> vi;

vi kmp(const vi &s, const vi &w) {
  int slen = s.size(),
      wlen = w.size();
  vi t(wlen+1),
     match;
  t[0] = -1;

  for (int i = 1; i <= wlen; i++) {
    int pos = t[i-1];
    while (pos > -1 && w[pos] != w[i-1])
      pos = t[pos];
    t[i] = pos + 1;
  }

  int sp = 0;
  int wp = 0;
  while (sp < slen) {
    while (wp > -1 && (wp == wlen || w[wp] != s[sp]))
      wp = t[wp];
    sp++;
    wp++;
    if (wp == wlen) match.push_back(sp - wlen);
  }

  return match;
}

int main() {
  ios_base::sync_with_stdio(false); cin.tie(NULL);

  int n, w, curr, prev;
  cin >> n >> w;
  if (w == 1) {
    cout << n << endl;
    return 0;
  }
  if (w > n) {
    cout << 0 << endl;
    return 0;
  }

  vi bear,
     elephant;
  cin >> prev;
  for (int i = 1; i < n; i++) {
    cin >> curr;
    bear.push_back(curr-prev);
    prev = curr;
  }
  cin >> prev;
  for (int i = 1; i < w; i++) {
    cin >> curr;
    elephant.push_back(curr-prev);
    prev = curr;
  }

  cout << kmp(bear, elephant).size() << endl;

  return 0;
}
