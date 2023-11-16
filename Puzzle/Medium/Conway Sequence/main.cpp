#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main() {
  int R;
  cin >> R;
  cin.ignore();
  int L;
  cin >> L;
  cin.ignore();

  std::vector<int> current;
  std::vector<int> next;

  current.push_back(R);
  for (unsigned i = 2; i <= L; ++i) {
    int cnt = 1;
    int numb = current[0];
    unsigned siz = current.size();
    for (auto j = 0; j < siz; ++j) {
      if (current[j] != numb) {
        next.push_back(cnt);
        next.push_back(numb);
        cnt = 1;
        numb = current[j];
      } else if (j > 0) {
        cnt++;
      }
    }
    next.push_back(cnt);
    next.push_back(numb);
    next.swap(current);
    next.clear();
  }

  unsigned siz = current.size();
  for (unsigned i = 0; i < siz - 1; ++i) {
    std::cout << current[i] << ' ';
  }
  std::cout << current.back() << std::endl;
}