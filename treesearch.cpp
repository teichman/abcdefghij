#include <iostream>
#include <cstdint>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "cxxopts.hpp"
#include <boost/multiprecision/cpp_int.hpp>

using namespace std;

//typedef uint64_t BigUInt;
//typedef unsigned __int128 BigUInt;  // This one doesn't know how to print itself.
//typedef boost::multiprecision::uint128_t BigUInt;  // This gets us to base 28.
typedef boost::multiprecision::uint256_t BigUInt;
//typedef boost::multiprecision::uint1024_t BigUInt;

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
  out << vec[0];
  for (size_t i = 1; i < vec.size(); ++i)
    out << ", " << vec[i];

  return out;
}

class TreeSearch
{
public:  
  TreeSearch(uint16_t base) : base_(base), value_(0)
  {
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

  void search()
  {
    // If there's only one digit left, and we know it has to be zero.
    if (digits_.size() == place_values_.size() - 1) {
      digits_.push_back(0);
      value_ = digits2val(digits_);

      cout << "Solution: " << digits_ << " (" << value_ << ")" << endl;
      assert(value_ % digits_.size() == 0);

      digits_.resize(digits_.size() - 1);
      value_ = digits2val(digits_);
      return;
    }

    // Otherwise, try adding digits that haven't been used yet, and recursively search if they work.
    for (size_t digit = 0; digit < place_values_.size(); ++digit) {
      if (used_[digit])
        continue;
      
      digits_.push_back(digit);
      used_[digit] = true;
      value_ = digits2val(digits_);
      
      if (value_ % digits_.size() == 0)
        search();
      
      digits_.resize(digits_.size() - 1);
      used_[digit] = false;
      value_ = digits2val(digits_);
    }
  }
    
private:
  uint16_t base_;
  BigUInt value_;
  vector<BigUInt> place_values_;
  vector<uint16_t> digits_;
  vector<bool> used_;
  
  BigUInt digits2val(const std::vector<uint16_t>& digits)
  {
    BigUInt val = 0;
    size_t idx = place_values_.size() - digits.size();
    for (size_t i = 0; i < digits.size(); ++i, ++idx)
      val += digits[i] * place_values_[idx];

    return val;
  }
};


int main(int argc, char** argv)
{
  cxxopts::Options options("BaseNum", "Conway's abcdefghij puzzle, but in bases other than 10.");
  options.add_options()
    ("b,base", "Base", cxxopts::value<int>())
    ;

  auto opts = options.parse(argc, argv);

  uint16_t base = opts["base"].as<int>();
  cout << "Evaluating on base " << base << endl;
  TreeSearch ts(base);
  ts.search();
}
