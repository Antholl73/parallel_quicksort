#include <iostream>
#include <vector>
using namespace std;

long min(const long &a, const long &b) { return (a < b) ? a : b; }

void merge(vector<int> &v, vector<int> &temp, long left_start, long mid,
           long right_end) {
  long i = left_start;
  long j = mid + 1;
  long k = left_start;

  while (i <= mid && j <= right_end) {
    if (v[i] <= v[j]) {
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

void mergesort(vector<int> &v, long init, long end) {
  if (init >= end) {
    return;
  }

  long size = end - init + 1;
  vector<int> temp(v.size());

  for (long curr_size = 1; curr_size < size; curr_size = curr_size * 2) {
    for (long left_start = init; left_start < end;
         left_start += curr_size * 2) {
      long mid = min(left_start + curr_size - 1, end);
      long right_end = min(left_start + 2 * curr_size - 1, end);

      merge(v, temp, left_start, mid, right_end);
    }
  }
}

int main(int argc, char *argv[]) {
  vector<int> arr = {38, 27, 43, 3, 9, 82, 10, 19};

  cout << "Arreglo original: ";
  for (int num : arr)
    cout << num << " ";
  cout << "\n";

  // Ahora pasamos el índice inicial (0) y el índice final (size - 1)
  mergesort(arr, 0, 7);

  cout << "Arreglo ordenado: ";
  for (int num : arr)
    cout << num << " ";
  cout << "\n";
  return 0;
}
