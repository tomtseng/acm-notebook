/*
 * https://www.topcoder.com/community/data-science/data-science-tutorials/assignment-problem-and-hungarian-algorithm/
 * http://www.cse.ust.hk/~golin/COMP572/Notes/Matching.pdf
 *
 * Problem Description
 * -------------------
 * SPOJ -- SCITIES
 */
#include <bits/stdc++.h>
using namespace std;

#define DEBUG
#ifdef DEBUG
#define TRACE(x) cerr << #x << " = " << x << endl;
#define _ << " _ " <<
#else
#define TRACE(x) ((void)0)
#endif

const int MAXN = 100;
const int INF = 1000000000;
int cost[MAXN][MAXN];
int labell[MAXN];
int labelr[MAXN];
int slackr[MAXN];
int slacki[MAXN];
int matchl[MAXN];
int matchr[MAXN];
int parr[MAXN];
int q[MAXN];
int qlo, qhi;
bool S[MAXN];
bool T[MAXN];
int NN;

bool tryextend(int s, int t) {
  if (matchr[t] == -1) {
    matchr[t] = s;
    int next = matchl[s];
    matchl[s] = t;
    while (next != -1) {
      const int tmp = next;
      matchr[next] = parr[next];
      next = matchl[parr[next]];
      matchl[parr[tmp]] = tmp;
    }
    return true;
  } else {
    const int news = matchr[t];
    S[news] = true;
    T[t] = true;
    parr[t] = s;
    q[qhi++] = news;

    for (int k = 0; k < NN; k++)
      if (labell[news] + labelr[k] - cost[news][k] < slackr[k])
        slackr[k] = labell[news] + labelr[k] - cost[news][k],
        slacki[k] = news;
    return false;
  }
}

void augment() {
  fill(S, S + NN, false);
  fill(T, T + NN, false);
  fill(parr, parr + NN, -1);
  qlo = 0; qhi = 0;

  int root;
  for (root = 0; root < NN; root++)
    if (matchl[root] == -1) {
      S[root] = true;
      q[qhi++] = root;
      break;
    }

  for (int j = 0; j < NN; j++)
    slackr[j] = labell[root] + labelr[j] - cost[root][j],
    slacki[j] = root;

  while(1) {
    while (qlo < qhi) {
      int curr = q[qlo++];
      for (int j = 0; j < NN; j++) {
        if (!T[j] && labell[curr] + labelr[j] == cost[curr][j]
            && tryextend(curr, j))
          return;
      }
    }

    qlo = qhi = 0;
    int delta = INF;
    for (int j = 0; j < NN; j++)
      if (!T[j] && slackr[j] < delta)
        delta = slackr[j];
    for (int i = 0; i < NN; i++)
      if (S[i])
        labell[i] -= delta;
    for (int j = 0; j < NN; j++) {
      if (T[j])
        labelr[j] += delta;
      else
        slackr[j] -= delta;
    }
    for (int j = 0; j < NN; j++)
      if (!T[j] && slackr[j] == 0 && tryextend(slacki[j], j))
        return;
  }
}

int hungarian(int n) {
  NN = n;
  fill(matchl, matchl + NN, -1);
  fill(matchr, matchr + NN, -1);
  fill(labelr, labelr + NN, 0);

  for (int i = 0; i < NN; i++) {
    int m = -INF;
    for (int j = 0; j < NN; j++)
      if (cost[i][j] > m)
        m = cost[i][j];
    labell[i] = m;
  }

  for (int i = 0; i < NN; i++)
    augment();

  int costsum = 0;
  for (int i = 0; i < NN; i++)
    costsum += cost[i][matchl[i]];
  return costsum;
}

int main() {
  ios_base::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);

  int t;
  cin >> t;
  while (t--) {
    fill(&cost[0][0], &cost[0][0] + sizeof(cost) / sizeof(cost[0][0]), 0);
    int n1, n2;
    cin >> n1 >> n2;
    while (1) {
      int c1, c2, g;
      cin >> c1 >> c2 >> g;
      if (c1 == 0)
        break;
      cost[c1 - 1][c2 - 1] = g;
    }
    cout << hungarian(max(n1, n2)) << '\n';
  }

  return 0;
}
