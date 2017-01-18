/*
 * This only supports alphanumeric characters.
 *
 * Problem description
 * -------------------
 * SPOJ: SUB_PROB
 * The test cases on this problem are weak. I submitted a solution that didn't
 * deal with some dictionary strings being substrings of one another, and it
 * passed. Hopefully this implementation below deals with that correctly, but it
 * hasn't been tested properly.
 */
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

struct LLnode {
  int hitidx;
  LLnode *next;

  LLnode(int _hitidx, LLnode *_next) : hitidx(_hitidx), next(_next) {}
};

struct LL {
  LLnode *head,
         *tail;

  LL() : head(nullptr), tail(nullptr) {}

  void push_front(int hitidx) {
    head = new LLnode(hitidx, head);
    if (tail == nullptr)
      tail = head;
  }
  void concat(const LL& rhs) {
    if (head == nullptr) {
      *this = rhs;
    } else if (rhs.head != nullptr) {
      tail->next = rhs.head;
      tail = rhs.tail;
    }
  }
};

struct Tnode {
  Tnode *child[62];
  Tnode *fail;
  LL hits;

  Tnode() : hits() {
    fill(child, child + 62, nullptr);
  }
};

struct Am {
  Tnode root;
  int convc[256];

  Am() : root() {
    for (int c = 'a'; c <= 'z'; c++)
      convc[c] = c - 'a';
    for (int c = 'A'; c <= 'Z'; c++)
      convc[c] = c - 'A' + 26;
    for (int c = '0'; c <= '9'; c++)
      convc[c] = c - '0' + 52;
  }

  // compute_fail() must be called after all insertions are done
  void insert(const string& s, int id) {
    Tnode *curr = &root;
    for (int c : s) {
      if (curr->child[convc[c]] == nullptr)
        curr->child[convc[c]] = new Tnode();
      curr = curr->child[convc[c]];
    }
    curr->hits.push_front(id);
  }

  void compute_fail() {
    root.fail = &root;
    queue<Tnode *> q;
    for (int i = 0; i < 62; i++)
      if (root.child[i] != nullptr) {
        root.child[i]->fail = &root;
        root.child[i]->hits.concat(root.hits);
        q.push(root.child[i]);
      }

    while (!q.empty()) {
      Tnode *v = q.front();
      q.pop();
      for (int i = 0; i < 62; i++)
        if (v->child[i] != nullptr) {
          Tnode *next = v->child[i];
          Tnode *f = v->fail;
          while (f != f->fail && f->child[i] == nullptr)
            f = f->fail;
          next->fail = f->child[i] == nullptr ? f : f->child[i];
          next->hits.concat(next->fail->hits);
          q.push(next);
        }
    }
  }

  Tnode *step(Tnode *v, int ch) {
    ch = convc[ch];
    while (v != v->fail && v->child[ch] == nullptr)
      v = v->fail;
    return v->child[ch] == nullptr ? v : v->child[ch];
  }
};

Am dfa;
string hay;
bool ans[1005];

int main() {
  ios_base::sync_with_stdio(false); cin.tie(nullptr); cout.tie(nullptr);

  int needles;
  cin >> hay >> needles;
  for (int i = 0; i < needles; i++) {
    string s;
    cin >> s;
    dfa.insert(s, i);
  }
  dfa.compute_fail();

  Tnode *curr = &dfa.root;
  for (auto c : hay) {
    curr = dfa.step(curr, c);
    // This is potentially O(n^2) if the input string is "aaaaaaaaaaaaaaaa" and
    // the dictionary strings are "a", "aa", "aaa", .... I think you can fix
    // this by breaking if ans[node->hitidx] is already set.
    for (LLnode *node = curr->hits.head; node != nullptr; node = node->next)
      ans[node->hitidx] = true;
  }
  for (int i = 0; i < needles; i++)
    cout << (ans[i] ? 'Y' : 'N') << '\n';

  return 0;
}
