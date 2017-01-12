/* 
 * Crude Bignum class for non-negative integers. Doesn't support many operations
 * and is quite fragile. Nonetheless, it might be useful in a pinch.
 * TODO: clean this up and extend it
 *
 * Problem description
 * -------------------
 * USACO training pages: vans
 * I came up with a way to solve this by deriving a recurrence, encoding this
 * recurrence as an 8x8 matrix, and then doing fast exponentiation. It's the
 * same idea as is given in the USACO solution.
 * But I didn't actually want to code that up, so instead I cheated by looking
 * up a simpler recurrence through  OEIS. In retrospect, this wasn't much easier
 * to implement since I had to additionally implement multiplication and
 * subtraction for big numbers.
 */
#include <bits/stdc++.h>
using namespace std;

// Represents large non-negative integers. Supports adding and multiplication.
class BigNum {
  public:
  BigNum();
  BigNum(long long val);

  string ToString() const;

  BigNum& operator=(const BigNum& rhs);
  BigNum& operator+=(const BigNum& rhs);
  // undefined behavior if lhs < rhs
  BigNum& operator-=(const BigNum& rhs);
  BigNum& operator*=(const BigNum& rhs);
  BigNum operator+(const BigNum& rhs) const;
  // undefined behavior if lhs < rhs
  BigNum operator-(const BigNum& rhs) const;
  BigNum operator*(const BigNum& rhs) const;

  private:
  void shift(size_t shift_amount);
  vector<long long> chunks_;
  const long long max_chunk_ = 1000000000;
  const size_t chunk_len_ = 9;
};

BigNum::BigNum() {}

BigNum::BigNum(long long val) {
  while (val) {
    chunks_.push_back(val % max_chunk_);
    val /= max_chunk_;
  }
}

string BigNum::ToString() const {
  if (chunks_.size() == 0)
    return "0";
  string ans;
  ans.reserve(chunk_len_ * chunks_.size());
  auto rit = chunks_.rbegin();
  ans += to_string(*rit);
  ++rit;
  for (; rit != chunks_.rend(); ++rit) {
    string chunk_str = to_string(*rit);
    ans += string(chunk_len_ - chunk_str.size(), '0') + chunk_str;
  }
  return ans;
}

BigNum& BigNum::operator=(const BigNum& rhs) {
  if (this != &rhs)
    chunks_ = rhs.chunks_;
  return *this;
}

BigNum& BigNum::operator+=(const BigNum& rhs) {
  const size_t lhs_len = chunks_.size();
  const size_t rhs_len = rhs.chunks_.size();
  if (lhs_len < rhs_len)
    chunks_.resize(rhs_len);

  size_t i = 0;
  long long carry = 0;
  for (; i < rhs_len; i++) {
    long long new_chunk = chunks_[i] + rhs.chunks_[i] + carry;
    chunks_[i] = new_chunk % max_chunk_;
    carry = new_chunk / max_chunk_;
  }
  while (i < lhs_len && carry) {
    long long new_chunk = chunks_[i] + carry;
    chunks_[i] = new_chunk % max_chunk_;
    carry = new_chunk / max_chunk_;
    i++;
  }
  if (carry)
    chunks_.push_back(carry);

  return *this;
}

BigNum& BigNum::operator-=(const BigNum& rhs) {
  const size_t lhs_len = chunks_.size();
  const size_t rhs_len = rhs.chunks_.size();

  for (size_t i = 0; i < rhs_len; i++)
    chunks_[i] -= rhs.chunks_[i];
  for (size_t i = 0; i < lhs_len - 1; i++)
    if (chunks_[i] < 0) {
      chunks_[i] += max_chunk_;
      chunks_[i + 1]--;
    }
  while (!chunks_.empty() && chunks_.back() == 0)
    chunks_.pop_back();

  return *this;
}

BigNum& BigNum::operator*=(const BigNum& rhs) {
  BigNum curr;
  size_t lhs_len = chunks_.size();
  size_t rhs_len = rhs.chunks_.size();
  for (size_t i = 0; i < rhs_len; i++) {
    BigNum scratch = *this;
    long long carry = 0;
    size_t j = 0;
    for (; j < lhs_len; j++) {
      long long new_chunk = scratch.chunks_[j] * rhs.chunks_[i] + carry;
      scratch.chunks_[j] = new_chunk % max_chunk_;
      carry = new_chunk / max_chunk_;
    }
    if (carry)
      scratch.chunks_.push_back(carry);
    scratch.shift(i);
    curr += scratch;
  }
  swap(chunks_, curr.chunks_);
  return *this;
}

BigNum BigNum::operator+(const BigNum& rhs) const {
  return BigNum(*this) += rhs;
}

BigNum BigNum::operator-(const BigNum& rhs) const {
  return BigNum(*this) -= rhs;
}

BigNum BigNum::operator*(const BigNum& rhs) const {
  return BigNum(*this) *= rhs;
}

void BigNum::shift(size_t shift_amount) {
  vector<long long> offset(shift_amount, 0);
  offset.reserve(shift_amount + chunks_.size());
  offset.insert(offset.end(), make_move_iterator(chunks_.begin()),
      make_move_iterator(chunks_.end()));
  swap(chunks_, offset);
}

int main() {
  ios_base::sync_with_stdio(false);
  ifstream fin("vans.in"); fin.tie(NULL);
  ofstream fout("vans.out"); fout.tie(NULL);

  BigNum* ans = new BigNum[1001];
  ans[0] = BigNum(0);
  ans[1] = BigNum(0);
  ans[2] = BigNum(2);
  ans[3] = BigNum(4);

  int n;
  fin >> n;
  for (int i = 4; i <= n; i++)
    ans[i] = BigNum(2) * (ans[i - 1] + ans[i - 2] - ans[i - 3]) + ans[i - 4];
  fout << ans[n].ToString() << endl;

  return 0;
}
