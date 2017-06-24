// Solves SPOJ ILKQUERYIII
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

const int MAXN = 1000000;

class Node {
 public:
  Node(pair<int, int> a[MAXN], int n);
  int quantile(int i, int l, int k);
  void swap(int i);

 private:
  Node(pair<int, int> a[MAXN], int l, int r, int sl, int sr);

  Node* c[2];
  vector<int> data;

  bool IsLeaf();
  int MapLeft(int i);
  int MapRight(int i);
  void modify_leaf(int i, int delta);
};

Node::Node(pair<int, int> a[MAXN], int n) : Node(a, 0, n, 0, n) {}

Node::Node(pair<int, int> a[MAXN], int l, int r, int sl, int sr)
  : data{} {
  if (sl + 1 == sr) {
    c[0] = c[1] = nullptr;
    data.reserve(r - l);
    for (int i = l; i < r; i++)
      data.push_back(a[i].second);
  } else {
    const int cut_pt = (sl + sr) / 2;
    data.reserve(r - l + 1);
    data.push_back(0);
    for (int i = l, acc = 0; i < r; i++) {
      if (a[i].first < cut_pt)
        acc++;
      data.push_back(acc);
    }
    auto it = stable_partition(a + l, a + r,
        [&](const pair<int, int>& x) {
          return x.first < cut_pt;
        }
      );
    int m = distance(a, it);
    c[0] = new Node(a, l, m, sl,     cut_pt);
    c[1] = new Node(a, m, r, cut_pt, sr);
  }
}

bool Node::IsLeaf() {
  return c[0] == nullptr;
}

int Node::MapLeft(int i) {
  return data[i];
}

int Node::MapRight(int i) {
  return i - data[i];
}

int Node::quantile(int i, int l, int k) {
  if (IsLeaf()) {
    return l >= (int)data.size() ? -1 : data[l];
  } else {
    return k < MapLeft(i) ? c[0]->quantile(MapLeft(i),  l, k) :
                            c[1]->quantile(MapRight(i), l, k - MapLeft(i));
  }
}

void Node::modify_leaf(int i, int delta) {
  if (IsLeaf()) {
    data[i] += delta;
  } else {
    if (data[i] == data[i + 1])
      c[1]->modify_leaf(MapRight(i), delta);
    else
      c[0]->modify_leaf(MapLeft(i), delta);
  }
}

void Node::swap(int i) {
  if (!IsLeaf()) {
    const int i_right  = data[i]     == data[i + 1];
    const int i1_right = data[i + 1] == data[i + 2];
    if (i_right + i1_right == 0) {
      c[0]->swap(MapLeft(i));
    } else if (i_right + i1_right == 2) {
      c[1]->swap(MapRight(i));
    } else {
      modify_leaf(i, 1);
      modify_leaf(i + 1, -1);
      if (i_right == 1)
        data[i + 1]++;
      else
        data[i + 1]--;
    }
  }
}

pair<int, int> A[MAXN];
int scratch[MAXN];
unordered_map<int, int> conv;

int main() {
  ios_base::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);

  int n, q;
  cin >> n >> q;
  for (int i = 0; i < n; i++) {
    int x;
    cin >> x;
    A[i] = make_pair(x, i);
    scratch[i] = x;
  }
  sort(scratch, scratch + n);
  int alphabet_sz = 0;
  for (int i = 0; i < n; i++)
    if (i == 0 || scratch[i] != scratch[i - 1])
      conv[scratch[i]] = alphabet_sz++;
  for (int i = 0; i < n; i++)
    A[i].first = conv[A[i].first];

  Node* root = new Node(A, n);
  for (int qq = 0; qq < q; qq++) {
    int ty, i, l, k;
    cin >> ty >> i;
    if (ty == 0) {
      cin >> l >> k;
      cout << root->quantile(i + 1, l - 1, k - 1) << '\n';
    } else {
      root->swap(i);
    }
  }

  return 0;
}
