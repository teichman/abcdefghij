#include <iostream>
#include <cstdint>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "cxxopts.hpp"

using namespace std;

uint64_t ipow(uint64_t base, uint64_t exp)
{
  uint64_t result = 1;
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

class BaseNum
{
public:
  BaseNum(uint16_t base) : base_(base)
  {
    // Note that our use of next_permutation means digits_ must be sorted
    // or we will silently get the wrong answer.
    digits_.resize(base_);
    for (size_t i = 0; i < digits_.size(); ++i)
      digits_[i] = i;

    exps_.resize(base_);
    for (size_t i = 0; i < exps_.size(); ++i)
      exps_[i] = ipow(base_, exps_.size() - i - 1);
    
    // if (base == 13) {
    //   digits_ =  {4, 8, 9, 3, 5, 7, 0, 12, 6, 10, 1, 11, 2};      
    //   cout << "Using special initial assignment." << endl;
    // }

    val_ = digits2val();

    // if (base == 13) 
    //   cout << status() << endl;
  }

  BaseNum(uint16_t base, const std::vector<uint16_t>& digits) : base_(base), digits_(digits)
  {
    exps_.resize(base_);
    for (size_t i = 0; i < exps_.size(); ++i)
      exps_[i] = ipow(base_, exps_.size() - i - 1);
    
    val_ = digits2val();
  }

  bool nextPermutation()
  {
    bool flag = std::next_permutation(digits_.begin(), digits_.end());
    val_ = digits2val();
    return flag;
  }

  // Check if this one satisfies the rule, and recursively call on its prefixes.
  bool isSolution() const
  {
    if (digits_.size() == 1)
      return (digits_[0] != 0);
    
    if (val_ % digits_.size() != 0)
      return false;

    return prefix().isSolution();
  }
  
  uint64_t val() const { return val_; }

  // Chop off the last digit.
  BaseNum prefix() const
  {
    vector<uint16_t> newdigits(digits_.begin(), digits_.begin() + digits_.size() - 1);
    return BaseNum(base_, newdigits);
  }

  std::string status(const std::string& prefix = "") const
  {
    ostringstream oss;
    oss << prefix << "Base " << base_ << ": " << digits_ << " (" << val_ << ")  isSolution: " << isSolution();
    return oss.str();
  }
  
private:
  uint16_t base_;
  uint64_t val_;
  vector<uint16_t> digits_;
  vector<uint64_t> exps_;
  
  uint64_t digits2val()
  {
    uint64_t val = 0;
    size_t eidx = exps_.size() - digits_.size();
    for (size_t i = 0; i < digits_.size(); ++i, ++eidx)
      val += digits_[i] * exps_[eidx];

    return val;
  }
};


int main(int argc, char** argv)
{
  cxxopts::Options options("BaseNum", "Conway's abcdefghij puzzle, but in bases other than 10.");
  options.add_options()
    ("b,base", "Base", cxxopts::value<int>())
    ;
    // ("d,debug", "Enable debugging") // a bool parameter
    // ("f,file", "File name", cxxopts::value<std::string>())
    // ("v,verbose", "Verbose output", cxxopts::value<bool>()->default_value("false"))

  auto opts = options.parse(argc, argv);

  uint16_t base = opts["base"].as<int>();
  cout << "Evaluating on base " << base << endl;
  BaseNum bn(base);
  do {
    if (bn.isSolution())
      cout << bn.status() << endl;
  } while (bn.nextPermutation());  
}
