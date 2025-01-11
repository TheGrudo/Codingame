#include <iostream>
#include <vector>

// Funkcja obliczająca liczbę różnych schodów z N cegieł
int countStaircases(int N)
{
    // Tablica dp[i][j] przechowuje liczbę sposobów na ułożenie schodów
    // z sumą cegieł równą i, przy czym największy stopień ma wysokość co najwyżej j.
    std::vector<std::vector<int>> dp(N + 1, std::vector<int>(N + 1, 0));

    // Warunek bazowy: istnieje jeden sposób na zbudowanie schodów o sumie 0 (brak cegieł)
    dp[0][0] = 1;

    // Dynamiczne wypełnianie tablicy dp
    for (int bricks = 1; bricks <= N; ++bricks)
    {
        for (int maxStep = 1; maxStep <= N; ++maxStep)
        {
            // Sposoby na ułożenie schodów bez użycia "maxStep"
            dp[bricks][maxStep] = dp[bricks][maxStep - 1];

            // Jeśli używamy "maxStep", sprawdzamy czy da się dodać do istniejącego rozwiązania
            if (bricks >= maxStep)
            {
                dp[bricks][maxStep] += dp[bricks - maxStep][maxStep - 1];
            }
        }
    }

    // Wynik to liczba sposobów na zbudowanie schodów o sumie N z maksymalnym stopniem <= N
    return dp[N][N];
}

int main()
{
    int N;
    std::cout << "Enter the number of bricks: ";
    std::cin >> N;

    int result = countStaircases(N);
    std::cout << "Number of different staircases that can be built: " << result << std::endl;

    return 0;
}
