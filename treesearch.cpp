#include <iostream>
#include <cstdint>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "cxxopts.hpp"
#include <boost/multiprecision/cpp_int.hpp>
#include <Eigen/Eigen>

using namespace std;
using namespace Eigen;

typedef boost::multiprecision::checked_uint1024_t BigUInt;

BigUInt ipow(BigUInt base, BigUInt exp)
{
  BigUInt result = 1;
  for (int i = 0; i < exp; ++i)
    result *= base;
  return result;
}

template<typename T>
ostream& operator<<(ostream& out, const vector<T>& vec)
{
  if (vec.empty())
    return out;
  
  out << vec[0];
  for (size_t i = 1; i < vec.size(); ++i)
    out << ", " << vec[i];

  return out;
}

class TreeSearch
{
public:  
  TreeSearch(uint16_t base, bool heuristic) : base_(base), heuristic_(heuristic), value_(0), num_evals_(0)
  {
    mat_ = MatrixXi::Zero(base_, base_);
    
    used_.resize(base, false);
    used_[0] = true;  // Reserve this for the last digit.

    // Fill in place_values_ and check for overflow in them.
    bool overflow = false;
    place_values_.resize(base_);
    for (size_t i = 0; i < place_values_.size(); ++i) {
      place_values_[i] = ipow(base_, place_values_.size() - i - 1);
      if (i > 0 && place_values_[i] > place_values_[i-1])
        overflow = true;
    }

    if (overflow) {
      cout << "Overflow detected." << endl;
      cout << "Place values:" << endl;
      for (size_t i = 0; i < place_values_.size(); ++i)
        cout << place_values_[i] << endl;
      assert(false);
    }
    
    // Also check for overflow if we form the max allowed number.
    // Not guaranteed...
    BigUInt val = 0;
    for (int i = 0; i < base_; ++i) {
      BigUInt prev_val = val;
      val += i * place_values_[base_ - i - 1];
      assert(val >= prev_val);
    }
  }

  void push(uint16_t digit)
  {
    digits_.push_back(digit);
    used_[digit] = true;

    // Zero is a special case that is only tried as the last digit.
    if (digit == 0) {
      assert((uint16_t)digits_.size() == base_);
      mat_(base_-1, base_-1) = 1;
    }
    else 
      mat_(digit-1, digits_.size()-1) = 1;
    
    value_ = digits2Val(digits_);
    num_evals_++;
  }

  void pop()
  {
    uint16_t digit = digits_.back();
    if (digit == 0)
      mat_(base_-1, base_-1) = 0;
    else
      mat_(digit-1, digits_.size()-1) = 0;
    
    digits_.resize(digits_.size() - 1);
    used_[digit] = false;
    value_ = digits2Val(digits_);
  }
  
  BigUInt search()
  { 
    // If there's only one digit left, we know it has to be zero.
    if (digits_.size() == place_values_.size() - 1) {
      push(0);

      if (value_ % digits_.size() == 0) {
        MatrixXi m = digits2Matrix(digits_);
        assert(m.isApprox(mat_));
        cout << "Solution: " << digits_ << " (" << value_ << ")"
             << " [Symmetric: " << m.isApprox(m.transpose()) << "]" << endl;
      }
      
      pop();
      return num_evals_;
    }

    // Otherwise, try adding digits that haven't been used yet, and recursively search if they work.
    // We know zero is always the last digit, so don't bother searching over that.
    for (size_t digit = 1; digit < place_values_.size(); ++digit) {
      if (used_[digit])
        continue;

      // In heuristic search, we only try alternating odd / even sequences.
      if (heuristic_)
        if (digits_.size() % 2 == digit % 2)
          continue;

      push(digit);

      // Check the main divisibility constraint.
      // If we pass, and we're doing an exhaustive search, then continue searching down this branch.
      // If we pass, and we're doing a heuristic search, check for symmetry first.
      if (value_ % digits_.size() == 0) {
        if (!heuristic_)
          search();
        else {
          const MatrixXi& block = mat_.block(0, 0, digits_.size(), digits_.size());
          if (block.isApprox(block.transpose()))
            search();
        }
      }

      pop();
    }

    return num_evals_;
  }
    
private:
  uint16_t base_;
  bool heuristic_;
  BigUInt value_;
  vector<BigUInt> place_values_;
  vector<uint16_t> digits_;  // in order of most significant to least significant
  vector<bool> used_;
  BigUInt num_evals_;
  // Matrix form of the solution so far.
  // Each column corresponds to one digit.
  // Is a fixed size base_ x base_, all zeros to start, with ones filled in as digits_ grows.
  MatrixXi mat_;
  
  BigUInt digits2Val(const std::vector<uint16_t>& digits)
  {
    BigUInt val = 0;
    size_t idx = place_values_.size() - digits.size();
    for (size_t i = 0; i < digits.size(); ++i, ++idx)
      val += digits[i] * place_values_[idx];

    return val;
  }

  MatrixXi digits2Matrix(const std::vector<uint16_t>& digits)
  {
    MatrixXi m = MatrixXi::Zero(digits.size(), digits.size());
    for (size_t i = 0; i < digits.size(); ++i) {
      // Ignore out of bounds digits.
      if (digits[i] >= m.rows())
        continue;
      else if (digits[i] != 0)
        m(digits[i] - 1, i) = 1;
      else
        m(digits.size() - 1, i) = 1;
    }
    return m;
  }
};

void symmetryCheck()
{
  MatrixXd m(2, 2);
  // m(r, c)
  m(0, 0) = 13;
  m(1, 0) = 42;
  cout << m << endl;
  cout << "Is symmetric: " << m.isApprox(m.transpose()) << endl;

  m(0, 0) = 13;
  m(1, 0) = 42;
  m(0, 1) = 42;
  m(1, 1) = 13;

  cout << m << endl;
  cout << "Is symmetric: " << m.isApprox(m.transpose()) << endl;

  int n = 4;
  MatrixXi big = MatrixXi::Zero(n, n);
  big(0, 1) = 1;
  big(1, 0) = 1;
  big(3, 2) = 1;
  big(2, 3) = 0;
  cout << "Submatrix test: " << endl;
  cout << big << endl;
 
  for (int sz = 1; sz <= n; ++sz) {
    cout << "Submatrix: " << endl;
    cout << big.block(0, 0, sz, sz) << endl;
    cout << "Is symmetric: " << big.block(0, 0, sz, sz).isApprox(big.block(0, 0, sz, sz).transpose()) << endl;
  }
}

int main(int argc, char** argv)
{
  cxxopts::Options optspec("treesearch", "Conway's abcdefghij puzzle, but in bases other than 10.\n");
  optspec.add_options()
    ("b,base", "What base to search", cxxopts::value<int>())
    ("heuristic", "Heuristic search (vs exhaustive search)")
    ("h,help", "Print usage")
    ;
  
  cxxopts::ParseResult opts;
  try {
    opts = optspec.parse(argc, argv);
  }
  catch (const cxxopts::OptionException& e) {
    cout << optspec.help() << endl;
    exit(0);
  }

  if (opts.count("help") || !opts.count("base")) {
    cout << optspec.help() << endl;
    exit(0);
  }
  
  uint16_t base = opts["base"].as<int>();
  bool heuristic = opts.count("heuristic");
  if (heuristic)
    cout << "Doing heuristic search on base " << base << ".  " 
         << "This won't find all answers, but can search higher bases." << endl;
  else
    cout << "Doing exhaustive search on base " << base << ".  " 
         << "If an answer exists, this should find it." << endl;
  
  TreeSearch ts(base, heuristic);
  BigUInt num_evals = ts.search();
  cout << "Num evals: " << num_evals << endl;
}
