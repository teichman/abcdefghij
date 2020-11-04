#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

int ipow(int base, int exp)
{
  int result = 1;
  for (;;)
  {
    if (exp & 1)
      result *= base;
    exp >>= 1;
    if (!exp)
      break;
    base *= base;
  }

  return result;
}

class Number
{
public:
  size_t val_;
  vector<int> digits_;

  Number(const vector<int>& digits) :
    digits_(digits)
  {
    val_ = 0;
    for (size_t i = 0; i < digits_.size(); ++i)
      val_ += digits_[i] * ipow(10, i);
  }
  
  Number(int val) :
    val_(val)
  {
    computeDigits();
  }

  Number(const Number& num) :
    val_(num.val_)
  {
    computeDigits();
  }


  Number biggestN(int n) const
  {
    vector<int> digits(digits_.end() - n, digits_.end());
    return Number(digits);
  }

  bool uniqueDigits() const
  {
    vector<int> d = digits_;
    std::sort(d.begin(), d.end());
    vector<int>::iterator ip = std::unique(d.begin(), d.end());
    d.resize(std::distance(d.begin(), ip));
    if (d.size() == digits_.size())
      return true;
    else
      return false;
  }

  bool isDivisibleBy(int n) const
  {
    return (val_ % n == 0);
  }
  
  size_t numDigits() const { return digits_.size(); }
  
  void computeDigits()
  {
    if (val_ == 0) {
      digits_.clear();
      digits_.push_back(0);
      return;
    }
    
    size_t val = val_;
    int num_digits = 0;
    while (val / ipow(10, num_digits) > 0)
    {
      ++num_digits;
    }

    digits_.resize(num_digits);

    for (int i = digits_.size() - 1; i >= 0; i--) {
      digits_[i] = val / ipow(10, i);
      val -= digits_[i] * ipow(10, i);
    }
  }

  int& operator[](int idx)
  {
    assert(idx >= 0);
    assert(idx < (int)digits_.size());
    return digits_[idx];
  }

  Number& operator++()
  {
    increment();
    return *this;
  }

  Number& operator*=(const Number& rhs)
  {
    val_ *= rhs.val_;
    return *this;
  }

  operator int() const { return val_; }
  
  void increment()
  {
    val_++;
    
    digits_[0]++;
    for (size_t i = 0; i < digits_.size(); ++i) {
      if (digits_[i] == 10) {
        digits_[i] = 0;
        if (i == digits_.size() - 1)
          digits_.push_back(1);
        else
          digits_[i+1]++;
      }
      
    }
  }
  
  std::string status(const std::string& prefix = "") const
  {
    ostringstream oss;
    oss << prefix << "val: " << val_ << " :: ";
    for (size_t i = 0; i < digits_.size(); ++i) {
      oss << digits_[i] << " ";
    }

    oss << endl;
    return oss.str();
  }
};

Number operator*(Number lhs, const Number& rhs)
{
  return lhs *= rhs;
}

Number operator*(int lhs, const Number& rhs)
{
  return lhs * rhs.val_;
}

bool operator==(const Number& lhs, const Number& rhs)
{
  return lhs.val_ == rhs.val_;
}

bool operator!=(const Number& lhs, const Number& rhs)
{
  return lhs.val_ != rhs.val_;
}

std::ostream& operator<<(std::ostream& os, const Number& num)
{
  os << num.val_;
  return os;
}

bool check(const Number& num) {
  if (!num.uniqueDigits())
    return false;
  
  for (int i = 1; i <= 10; ++i)
    if (!num.biggestN(i).isDivisibleBy(i))
      return false;
  return true;
}

void confirmSolution(const Number& num)
{
  for (int i = 1; i <= 10; ++i) {
    cout << num.biggestN(i).status();
    cout << "  Divisible by " << i << " :: " << num.biggestN(i).isDivisibleBy(i) << endl;
    cout << "  Dividing: " << std::setprecision(10) << std::fixed << (float)num.biggestN(i).val_ / (float)i << endl;
  }
}

int main(int argc, char** argv)
{
  for (Number num(1000000000); num.val_ <= 9999999999; ++num) {  
    if (num.val_ % 10000 == 0)
      cout << num << endl;
    if (check(num)) {
      cout << "SOLUTION!!  " << num << endl;
      confirmSolution(num);
      return 0;
    }
  }
}
