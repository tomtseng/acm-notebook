// Link-cut tree implementation.
// Solves DYNALCA and DYNACON1 on SPOJ.
// Path queries are untested.
#include <bits/stdc++.h>
using namespace std;

class Node {
 public:
  Node();
  Node(Node* _par, Node* left, Node *right, int _val);

  Node* get_root();
  void cut(Node* neighbor);
  void cut_from_par();
  void link(Node* child);
  Node* lca(Node* other);
  void evert(); // reroot
  int path_to_root_query();

 private:
  Node* par; // parent
  Node* c[2]; // children
  // node value and subtree accumulator value
  // accumulator function is assumed to be commutative. Otherwise, the
  // accumulator must be updated more frequently (once more after rotation, and
  // after flipping)
  int val, acc;
  bool flip; // whether children are reversed; used for evert()

  Node* get_real_par();
  void rot();
  void splay();
  Node* expose();
  void fix_c();
  void push_flip();
  void update_acc();
};

Node::Node(Node* _par, Node* left, Node *right, int _val)
  : par(_par), c{left, right}, val(_val), flip(0) {
  fix_c();
}

Node::Node() : Node(nullptr, nullptr, nullptr, 1) {}

Node* Node::get_real_par() {
  return par != nullptr && this != par->c[0] && this != par->c[1] ? nullptr : par;
}

void Node::update_acc() {
  acc = val + (c[0] == nullptr ? 0 : c[0]->acc)
            + (c[1] == nullptr ? 0 : c[1]->acc);
}

void Node::fix_c() {
  for (int i = 0; i < 2; i++)
    if (c[i] != nullptr)
      c[i]->par = this;
  update_acc();
}

void Node::push_flip() {
  if (flip) {
    flip = 0;
    swap(c[0], c[1]);
    for (int i = 0; i < 2; i++)
      if (c[i] != nullptr)
        c[i]->flip ^= 1;
  }
}

void Node::rot() { // rotate v towards its parent; v must have real parent
  Node* p = get_real_par();
  par = p->par;
  if (par != nullptr)
    for (int i = 0; i < 2; i++)
      if (par->c[i] == p) {
        par->c[i] = this;
        par->fix_c();
      }
  const bool rot_dir = this == p->c[0];
  p->c[!rot_dir] = c[rot_dir];
  c[rot_dir] = p;
  p->fix_c();
  fix_c();
}

void Node::splay() {
  Node* p, * gp;
  push_flip(); // guarantee flip bit isn't set after calling splay()
  while ((p = get_real_par()) != nullptr) {
    gp = p->get_real_par();
    if (gp != nullptr)
      gp->push_flip();
    p->push_flip();
    push_flip();
    if (gp != nullptr)
      ((gp->c[0] == p) == (p->c[0] == this) ? p : this)->rot();
    rot();
  }
}

// returns 1st vertex encountered that was originally in same path as root (used
// for LCA)
Node* Node::expose() {
  Node* ret = this;
  for (Node* curr = this, * pref = nullptr; curr != nullptr;
       ret = curr, pref = this, curr = par) {
    curr->splay();
    curr->c[1] = pref;
    curr->fix_c();
    splay();
  }
  return ret;
}

void Node::evert() {
  expose();
  flip ^= 1;
  push_flip();
}

Node* Node::get_root() {
  expose();
  Node* root = this;
  push_flip();
  while (root->c[0] != nullptr) {
    root = root->c[0];
    root->push_flip();
  }
  root->splay();
  return root;
}

void Node::cut_from_par() {
  expose();
  assert(c[0] != nullptr);
  c[0] = c[0]->par = nullptr;
  fix_c();
}

void Node::cut(Node* neighbor) {
  neighbor->evert();
  evert();
  assert(neighbor->par == this);
  neighbor->par = nullptr;
  for (int i = 0; i < 2; i++)
    if (c[i] == neighbor)
      c[i] = nullptr;
  fix_c();
}

void Node::link(Node* child) {
  assert(get_root() != child->get_root()); // no cycles
  child->evert();
  expose();
  child->par = this;
}

Node* Node::lca(Node* other) {
  expose();
  return other->expose();
}

int Node::path_to_root_query() {
  expose();
  return acc;
}

const int MAXN = 100001;
Node nodes[MAXN];

int main() {
  ios_base::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);

  fill_n(nodes, MAXN, Node());

  int n, m;
  cin >> n >> m;

  for (int i = 0; i < m; i++) {
    string s;
    int u, v;
    cin >> s >> u;
    switch (s[1]) {
      case 'i':
        cin >> v;
        nodes[v].link(&nodes[u]);
        break;
      case 'u':
        nodes[u].cut_from_par();
        break;
      case 'c':
        cin >> v;
        cout << distance(&nodes[0], nodes[u].lca(&nodes[v])) << '\n';
        break;
      default:
        assert(false);
    }
  }
  return 0;
}
