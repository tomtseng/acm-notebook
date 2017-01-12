/* 
 * http://www.cs.cmu.edu/afs/cs.cmu.edu/academic/class/15451-f14/www/lectures/lec27/fft-notes.pdf
 *
 * Problem Description
 * -------------------
 * NAIPC 2016: K-Inversions
 */
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
#define DEBUG
#ifdef DEBUG
#define TRACE(x) cerr << #x << " = " << x << endl;
#define _ << " _ " <<
#else
#define TRACE(x) ((void)0)
#endif

// Note: This assumes coefficients are non-negative

const ll N = 1 << 21; // power of 2 that strictly bounds degree of C
// Set this to be a bound on the largest possible coefficient in C, e.g. if
// values of A and B are bounded by b, then set bound = b ^ 2 * N
ll BOUND = 1 << 20;

// prime P such that P > bound and such that we have a principal N-th root
// of unity in F_P
const ll P = 23068673;
const ll ROOT = 177147;
const ll ROOTINV = 17187657;
const ll NINV = 23068662;

int A[N];
int Avals[N];
int B[N];
int Bvals[N];
int C[N]; // Coefficients of C(x) = A(x) * B(x)
int Cvals[N];

// The below code was used to find P, ROOT, ROOTINV, NINV
/*
bool is_prime(int x) {
  if (x % 2 == 0)
    return false;
  for (int i = 3; i * i <= x; i += 2)
    if (x % i == 0)
      return false;
  return true;
}

// x^e % m
int mexp(int x, int e, int m) {
  if (e == 0)
    return 1;
  const ll r = mexp(x, e / 2, m);
  if (e % 2 == 0)
    return (r * r)  % m;
  else
    return ((r * r) * x) % m;
}

// b^-1 mod m
int getinv(int b, int m) {
  b = b % m;
  int a = m;
  int x1 = 0, y1 = 1,
      x2 = 1, y2 = 0;
  while (b) {
    const int d = a / b;
    const int tmp1 = x1, tmp2 = y1;
    x1 = x2;
    y1 = y2;
    x2 = (((tmp1 - (ll)d * x2) % m) + m) % m;
    y2 = (((tmp2 - (ll)d * y2) % m) + m) % m;
    const int tmp = a;
    a = b;
    b = tmp % b;
  }
  return x1;
}

void findP() {
  int inv;
  for (int k = (BOUND + N - 1) / N; k >= 0; k++) {
    const int p = k * N + 1;
    if (is_prime(p)) {
      for (int j = 2; j < p; j++) {
        int g = mexp(j, k, p);
        unordered_set<int> seen;
        ll curr = 1;
        bool good = true;
        for (int i = 1; i <= N; i++) {
          curr = (curr * g) % p;
          if (i == N - 1)
            inv = curr;
          if (seen.find(curr) != seen.end()) {
            good = false;
            break;
          }
          seen.insert(curr);
        }
        if (good) {
          cerr << "P: " << p << '\n'
               << "ROOT: " << g << '\n'
               << "ROOTINV: " << inv << '\n'
               << "NINV: " << getinv(N, p) << '\n';
          return;
        }
      }
    }
  }
  assert(0);
}
*/

void fft(int root, int inl, int injump, int outl, int outn, int *inp, int *outp) {
  if (outn == 1) {
    outp[outl] = inp[inl];
    return;
  }
  const int newroot = ((ll)root * root) % P;
  const int newoutn = outn / 2;
  const int outl2 = outl + newoutn;
  fft(newroot, inl,          2 * injump, outl,  newoutn, inp, outp);
  fft(newroot, inl + injump, 2 * injump, outl2, newoutn, inp, outp);
  ll x = 1;
  for (int i = 0; i < newoutn; i++) {
    const int ev = outp[outl  + i];
    const int ov = outp[outl2 + i];
    outp[outl + i]  =   (ev + x * ov) % P;
    outp[outl2 + i] = (((ev - x * ov) % P) + P) % P;
    x = (x * root) % P;
  }
}

void convolve() {
  fft(ROOT, 0, 1, 0, N, A, Avals);
  fft(ROOT, 0, 1, 0, N, B, Bvals);
  for (int i = 0; i < N; i++)
    Cvals[i] = ((ll)Avals[i] * Bvals[i]) % P;
  fft(ROOTINV, 0, 1, 0, N, Cvals, C);
  for (int i = 0; i < N; i++)
    C[i] = ((ll)C[i] * NINV) % P;
}

int main() {
  ios_base::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);

  string s;
  cin >> s;
  int n = s.size();
  for (int i = 0; i < n; i++) {
    if (s[i] == 'A')
      B[n - i - 1] = 1;
    else
      A[i] = 1;
  }
  convolve();
  for (int i = n - 2; i >= 0; i--)
    cout << C[i] << '\n';

  return 0;
}
