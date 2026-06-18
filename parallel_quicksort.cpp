#include <functional>
#include <iostream>
#include <thread>
#include <utility>
#include <vector>
using namespace std;

// UTILS
void print_vector(std::vector<int> &v) {
  for (long i = 0; i < v.size(); i++) {
    std::cout << v[i] << " ";
  }
  std::cout << std::endl;
}

// STRUCT LESS GREATER
template <class data_type> struct Less {
  bool operator()(const data_type &a, const data_type &b) { return a < b; }
};

template <class data_type> struct Greater {
  bool operator()(const data_type &a, const data_type &b) { return a > b; }
};

// QUICKSORT
template <class data_type, class comparator>
long partition(vector<data_type> &v, long init, long end, comparator comp) {
  long i = init;
  long j = end;
  int pivot = v[init];

  while (i <= j) {
    while (comp(v[i], pivot)) {
      i++;
    }
    while (comp(pivot, v[j])) {
      j--;
    }

    if (i >= j) {
      return j;
    }

    swap(v[i], v[j]);
    i++;
    j--;
  }
  return j;
}

template <class data_type, class comparator>
void quicksort(vector<data_type> &v, long init, long end, comparator comp) {
  if (init >= end) {
    return;
  }
  long p = partition(v, init, end, comp);
  quicksort(v, init, p, comp);
  quicksort(v, p + 1, end, comp);
}

// MERGE
template <class data_type, class comparator>
void merge(vector<data_type> &v, vector<data_type> &temp, long left_start,
           long mid, long right_end, comparator comp) {
  long i = left_start;
  long j = mid + 1;
  long k = left_start;

  while (i <= mid && j <= right_end) {
    if (comp(v[i], v[j])) {
      temp[k] = v[i];
      i++;
    } else {
      temp[k] = v[j];
      j++;
    }
    k++;
  }

  while (i <= mid) {
    temp[k] = v[i];
    i++;
    k++;
  }
  while (j <= right_end) {
    temp[k] = v[j];
    j++;
    k++;
  }

  for (long idx = left_start; idx <= right_end; idx++) {
    v[idx] = temp[idx];
  }
}

// DIVIDE VECTOR IN RANGES

struct Range {
  long start;
  long end;
};

vector<Range> get_ranges(long size, long n_threads) {
  vector<Range> ranges;
  long part_base = size / n_threads;
  long module = size % n_threads;
  long curr_start = 0;

  for (long i = curr_start; i < n_threads; ++i) {
    long part_size = part_base + ((i < module) ? 1 : 0);

    if (part_size > 0) {
      long curr_end = curr_start + part_size - 1;

      ranges.push_back({curr_start, curr_end});

      curr_start = curr_start + part_size;
    }
  }
  return ranges;
}

// PARALLEL SORTS
template <class data_type, class comparator>
void parallel_sorts(vector<data_type> &v, long size, long n_threads,
                    comparator comp) {
  vector<Range> ranges = get_ranges(size, n_threads);
  vector<thread> threads;

  for (const auto &r : ranges) {
    threads.emplace_back(quicksort<data_type, comparator>, std::ref(v), r.start,
                         r.end, comp);
  }

  for (auto &t : threads) {
    if (t.joinable()) {
      t.join();
    }
  }
}

// PARALLEL MERGES

template <class data_type, class comparator>
void parallel_merge(vector<data_type> &v, vector<data_type> &temp,
                    vector<Range> ranges, comparator comp) {
  if (ranges.size() <= 1) {
    return;
  }

  vector<Range> next_level_ranges;
  vector<thread> merge_threads;

  for (long i = 0; i < (long)ranges.size(); i += 2) {
    if (i + 1 < (long)ranges.size()) {

      long left_start = ranges[i].start;
      long mid = ranges[i].end;
      long right_end = ranges[i + 1].end;

      merge_threads.emplace_back(merge<data_type, comparator>, std::ref(v),
                                 std::ref(temp), left_start, mid, right_end,
                                 comp);

      next_level_ranges.push_back(ranges[i]);
    } else {
      next_level_ranges.push_back(ranges[i]);
    }
  }

  for (auto &t : merge_threads) {
    if (t.joinable()) {
      t.join();
    }
  }

  parallel_merge(v, temp, next_level_ranges, comp);
}

int main(int argc, char *argv[]) {
  vector<int> v = {9,  3,  2,  6,  2,  32, 2,   5,   2,  6,  45, 24, 57,
                   42, 2,  14, 63, 68, 5,  4,   675, 95, 44, 34, 82, 19,
                   43, 65, 27, 89, 23, 44, 323, 12,  11, 34, 84, 46, 0};

  long init = 0;
  long end = v.size() - 1;

  print_vector(v);

  quicksort(v, init, end, Greater<int>());

  print_vector(v);

  return 0;
}
