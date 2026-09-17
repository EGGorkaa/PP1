#include <chrono>
#include <future>
#include <iostream>
#include <vector>

using data_t = std::vector< unsigned long long >;
using value_t = data_t::value_type;

class Clicker
{
public:
  using clock_t = std::chrono::steady_clock;
  Clicker():
    start_(clock_t::now())
  {}
  double millisec() const
  {
    auto t = clock_t::now();
    return std::chrono::duration< double, std::milli >(t - start_).count();
  }

private:
  clock_t::time_point start_;
};

value_t partSum(const data_t &arr, size_t begin, size_t end)
{
  int sum = 0;
  for (size_t i = begin; i < end; ++i) {
    sum += arr[i];
  }
  return sum;
}

value_t parallelSum(const data_t &arr, size_t chunkNum)
{
  size_t chunk = arr.size() / chunkNum;
  size_t restChunk = arr.size() % chunkNum;
  std::vector< std::future< value_t > > sums;
  size_t begin = 0;
  for (size_t i = 0; i < chunkNum; ++i) {
    sums.push_back(std::async(std::launch::async, partSum, std::cref(arr), begin, begin + chunk));
    begin += chunk;
  }
  if (restChunk != 0) {
    sums.push_back(std::async(std::launch::async, partSum, std::cref(arr), begin, begin + restChunk));
    begin += restChunk;
  }
  value_t totalSum = 0;
  for (size_t j = 0; j < sums.size(); ++j) {
    totalSum += sums[j].get();
  }
  return totalSum;
}

int main()
{
  constexpr size_t size{100'000'000};
  data_t values(size, 1);
  for (size_t j = 0; j < 5; ++j) {
    std::cout << j + 1 << " test\n";
    for (size_t n = 1; n <= 1024; n *= 2) {
      double init = 0, total = 0;
      value_t sum = 0;
      {
        Clicker cl;
        init = cl.millisec();
        sum = parallelSum(values, n);
        total = cl.millisec();
      }
      double ms = total - init;
      std::cout << n << "," << ms << "\n";
    }
  }
}