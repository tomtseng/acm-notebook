// These aren't actually used much in contest programming, but I wanted to
// implement it anyway.
// Solves SPOJ DYNACON1.
#include <bits/stdc++.h>
using namespace std;

class Node {
 public:
  int data;
  Node(int _data);
  Node(Node* _par, Node* left, Node *right, int _data);
  void del();

  Node* get_min();
  Node* get_max();
  void insert_max_node(Node* new_node);
  Node* get_successor();
  pair<Node*, Node*> split();
  static Node* join(Node* less, Node* great);
  // returns <pointer to a node in the same tree (or nullptr if tree is empty),
  //          pointer to deleted node (for deallocation, etc)>
  pair<Node*, Node*> delete_min();
  pair<Node*, Node*> delete_max();

 private:
  Node* par; // parent
  Node* c[2]; // children

  void assign_c(int i, Node* v);
  void rot();
  void splay();
  Node* get_extreme(int i);
  pair<Node*, Node*> delete_extreme(int i);
};

Node::Node(Node* _par, Node* left, Node *right, int _data)
  : data(_data), par(_par), c{left, right} {}

Node::Node(int _data) : Node(nullptr, nullptr, nullptr, _data) {}

void Node::del() {
  delete this;
}

void Node::assign_c(int i, Node* v) {
  if (v != nullptr)
    v->par = this;
  c[i] = v;
}

void Node::rot() { // rotate v towards its parent; v must have real parent
  Node* p = par;
  par = p->par;
  if (par != nullptr)
    for (int i = 0; i < 2; i++)
      if (par->c[i] == p)
        par->c[i] = this;
  const bool rot_dir = this == p->c[0];
  p->assign_c(!rot_dir, c[rot_dir]);
  assign_c(rot_dir, p);
}

void Node::splay() {
  Node* p, * gp;
  while ((p = par) != nullptr) {
    if ((gp = p->par) != nullptr)
      ((gp->c[0] == p) == (p->c[0] == this) ? p : this)->rot();
    rot();
  }
}

Node* Node::get_extreme(int i) {
  splay();
  Node* cur = this;
  while (cur->c[i] != nullptr)
    cur = cur->c[i];
  cur->splay();
  return cur;
}

Node* Node::get_min() {
  return get_extreme(0);
}

Node* Node::get_max() {
  return get_extreme(1);
}

Node* Node::get_successor() {
  splay();
  if (c[1] == nullptr)
    return nullptr;
  Node* cur = c[1];
  while (cur->c[0] != nullptr)
    cur = cur->c[0];
  return cur;
}

pair<Node*, Node*> Node::split() {
  splay();
  Node* left = c[0];
  if (left != nullptr)
    left->par = nullptr;
  c[0] = nullptr;
  return make_pair(left, this);
}

Node* Node::join(Node* less, Node* great) {
  if (less == nullptr)
    return great;
  if (great == nullptr)
    return less;
  less->splay();
  great->splay();
  assert(less->par == nullptr);
  Node* less_max = less->get_max();
  less_max->assign_c(1, great);
  return less_max;
}

void Node::insert_max_node(Node* new_node) {
  get_max()->assign_c(1, new_node);
}

pair<Node*, Node*> Node::delete_extreme(int i) {
  Node* to_delete = get_extreme(i);
  Node* tree_pointer = to_delete->c[!i];
  if (tree_pointer != nullptr)
    tree_pointer->par = to_delete->c[!i] = nullptr;
  return make_pair(tree_pointer, to_delete);
}

pair<Node*, Node*> Node::delete_min() {
  return delete_extreme(0);
}

pair<Node*, Node*> Node::delete_max() {
  return delete_extreme(1);
}

// pseudocode
// ----------
// split(v)
//   return [<v],[>=v]
//
// // For each vertex, we'll keep a pointer of an "active" occurrence of that
// // vertex. For each edge, we'll keep two pointers, one for each place the
// // edge appears in the Euler tour (we'll call these "active" for the edge
// // too). These pointers tell us where to split.
//
// reroot(r)
//   (A, B) = split(r)
//   delete min elem of A
//     if was active for v or an edge, reassign active items to be max elem of B
//   append r to A
//   return join(B, A)
//
// addedge(u, v)
//   reroot(v)
//     last vert of v's tree is active v for (u, v)
//   (A, B) = split(u)
//   append u to A
//     this is active u for (u,v)
//   A = join(A, v's tree)
//   return join(A, B)
//
// removeedge(u, v)
//   // TODO: can we get rid of this reroot?
//   reroot(u) (this is so the two occurrences of (u,v) appear in right order)
//   let u* be active u for (u,v), v* be active v for (u,v)
//   (A,B) = split(u*)
//   deletemin(B). if u* is active occurrence of u, set active[u] = succ(v*)
//   (_, C) = split(succ(v*))
//   join(A, C)

struct HashPairInt {
  size_t operator()(pair<int, int> p) const {
    static std::hash<unsigned> int_hash;
    const size_t h = int_hash(p.first);
    // taken from boost::hash_combine()
    return h ^ (int_hash(p.second) + 0x9e3779b9 + (h << 6) + (h >> 2));
  }
};

const int MAXN = 100001;
Node* active_vert[MAXN];
unordered_map<pair<int, int>, pair<Node*, Node*>, HashPairInt> active_edge;

void init() {
  for (int i = 0; i < MAXN; i++)
    active_vert[i] = new Node(i);
}

Node* append_node(Node* target, Node* new_node) {
  if (target == nullptr)
    return new_node;
  else {
    target->insert_max_node(new_node);
    return target;
  }
}

Node* append(Node* target, int data) {
  return append_node(target, new Node(data));
}


bool is_connected(int u, int v) {
  return active_vert[u]->get_min() == active_vert[v]->get_min();
}

void reroot(int r) {
  Node* ar = active_vert[r], * tA, * tB, * deleted_min, *deleted_max;
  tie(tA, tB) = ar->split();
  if (tA != nullptr) {
    tie(tA, deleted_min) = tA->delete_min();
    // to fix some bookkeeping with active_edge, we swap out the last element of
    // the tour with the first element even though their data should be the same
    tie(tB, deleted_max) = tB->delete_max();
    if (deleted_max == active_vert[deleted_max->data])
      active_vert[deleted_max->data] = deleted_min;
    delete deleted_max;
    tB = append_node(tB, deleted_min);
    tA = append(tA, r);
    Node::join(tB, tA);
  }
}

void add_edge(int u, int v) {
  if (u > v)
    swap(u, v);
  reroot(v);
  Node* edge_v = active_vert[v]->get_max();
  Node* tA, * tB;
  tie(tA, tB) = active_vert[u]->split();
  tA = append(tA, u);
  Node* edge_u = tA->get_max();
  tA = Node::join(tA, active_vert[v]);
  Node::join(tA, tB);
  active_edge[make_pair(u, v)] = make_pair(edge_u, edge_v);
}

void remove_edge(int u, int v) {
  if (u > v)
    swap(u, v);
  reroot(u);
  Node* edge_u, * edge_v, * tA, * tB, *tC, * deleted, * unused;
  tie(edge_u, edge_v) = active_edge[make_pair(u, v)];
  active_edge.erase(make_pair(u, v));
  tie(tA, tB) = edge_u->split();
  Node* succ_u = edge_v->get_successor();
  tie(tB, deleted) = tB->delete_min();
  if (deleted == active_vert[u])
    active_vert[u] = succ_u;
  delete deleted;
  tie(unused, tC) = succ_u->split();
  Node::join(tA, tC);
}

int main() {
  ios_base::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);

  init();

  int n, q;
  cin >> n >> q;
  for (int i = 0; i < q; i++) {
    string s;
    int u, v;
    cin >> s >> u >> v;
    switch (s[0]) {
      case 'a':
        add_edge(u, v);
        break;
      case 'r':
        remove_edge(u, v);
        break;
      case 'c':
        cout << (is_connected(u, v) ? "YES\n" : "NO\n");
        break;
    }
  }
  return 0;
}
