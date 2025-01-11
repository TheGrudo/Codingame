#undef _GLIBCXX_DEBUG                // disable run-time bound checking, etc
#pragma GCC optimize("Ofast,inline") // Ofast = O3,fast-math,allow-store-data-races,no-protect-parens

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

long countStaircasesMemo(long N, int minStep, vector<vector<long>> &memo)
{
    // Jeśli N jest równe 0, to jest tylko 1 sposób (brak schodków)
    if (N == 0)
        return 1;

    // Jeśli N jest mniejsze niż 0, nie ma możliwości (brak rozwiązania)
    if (N < 0)
        return 0;

    // Jeśli zapisaliśmy wynik, zwróć go z memo
    if (memo[N][minStep] != -1)
        return memo[N][minStep];

    // Rozważamy dwie możliwości: dodanie nowego kroku lub nie
    long result = 0;
    for (int step = minStep; step <= N; ++step)
    {
        result += countStaircasesMemo(N - step, step + 1, memo); // Dodajemy nowy krok
    }

    // Zapisujemy wynik w memo
    memo[N][minStep] = result;
    return result;
}

// Główna funkcja licząca liczbę schodków
long countStaircases(long N)
{
    vector<vector<long>> memo(N + 1, vector<long>(N + 1, -1)); // Memoizacja
    return countStaircasesMemo(N, 1, memo) - 1;                // Zaczynamy od pierwszego kroku
}
int main()
{
    int n;
    cin >> n;
    cin.ignore();

    long result = countStaircases(n);

    cout << result << endl;
}