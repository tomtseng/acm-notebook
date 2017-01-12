/*
 * This only supports lowercase characters.
 * Nodes are dynamically allocated. For slightly better performance, make them
 * statically allocated.
 *
 * Problem description
 * -------------------
 * USACO 2015 February Gold: Censoring
 * Note: this implementation times out on this problem because Aho-Corasick is
 * only linear time in an amortized sense, and here we use the data structure
 * "persistently." In this problem, it's not a hard fix, though.
 */
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

#define cin fin
#define cout fout
#define FNAME "censor"
ifstream fin(FNAME ".in");
ofstream fout(FNAME ".out");

struct Node {
  Node *child[26];
  Node *fail;
  int depth; // used in this problem to tell how long the matched string is
  bool hit;

  Node(int _depth) : depth(_depth), hit(false) {
    fill(child, child + 26, nullptr);
  }
};

struct Am {
  Node root;
  Am() : root(0) {}

  // compute_fail() must be called after all insertions are done
  void insert(const string& s) {
    Node *curr = &root;
    for (auto c : s) {
      if (curr->child[c - 'a'] == nullptr)
        curr->child[c - 'a'] = new Node(curr->depth + 1);
      curr = curr->child[c - 'a'];
    }
    curr->hit = true;
  }

  void compute_fail() {
    root.fail = &root;
    queue<Node *> q;
    for (int i = 0; i < 26; i++)
      if (root.child[i] != nullptr) {
        root.child[i]->fail = &root;
        q.push(root.child[i]);
      }

    while (!q.empty()) {
      Node *v = q.front();
      q.pop();
      for (int i = 0; i < 26; i++)
        if (v->child[i] != nullptr) {
          Node *next = v->child[i];
          Node *f = v->fail;
          while (f != f->fail && f->child[i] == nullptr)
            f = f->fail;
          next->fail = f->child[i] == nullptr ? f : f->child[i];
          q.push(next);
        }
    }
  }

  Node *step(Node *v, int ch) {
    ch -= 'a';
    while (v != v->fail && v->child[ch] == nullptr)
      v = v->fail;
    return v->child[ch] == nullptr ? v : v->child[ch];
  }
};

Am dfa;
string hay; // 1-indexed
Node *states[100001]; // states[i] is state of DFA immediately after processing hay[i]
int haynxt[100001];
int hayprv[100001];

int main() {
  ios_base::sync_with_stdio(false); fin.tie(nullptr); fout.tie(nullptr);

  int needles;
  cin >> hay >> needles;
  int n = hay.size();
  hay = '$' + hay;
  for (int i = 0; i < needles; i++) {
    string s;
    cin >> s;
    dfa.insert(s);
  }
  dfa.compute_fail();

  for (int i = 0; i <= n; i++)
    haynxt[i] = i + 1;
  for (int i = 0; i <= n; i++)
    hayprv[i] = i - 1;

  states[0] = &dfa.root;
  int idx = 0;
  while (idx <= n) {
    const int next = haynxt[idx];
    if (states[idx]->hit) {
      int bidx = idx;
      const int d = states[idx]->depth;
      for (int i = 0; i < d; i++)
        bidx = hayprv[bidx];
      haynxt[bidx] = next;
      hayprv[next] = bidx;
      if (next <= n)
        states[next] = dfa.step(states[bidx], hay[next]);
      idx = next;
    } else {
      if (next <= n)
        states[next] = dfa.step(states[idx], hay[next]);
      idx = next;
    }
  }
  for (int i = haynxt[0]; i <= n; i = haynxt[i])
    cout << hay[i];
  return 0;
}
