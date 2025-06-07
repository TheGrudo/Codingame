#include <cmath>
#include <iomanip>
#include <iostream>

double compute_h(double r, double R, double H, double V)
{
    double k = (R - r) / H;
    double a = (3 * r) / k;
    double b = (3 * r * r) / (k * k);
    double c = -(3 * V) / (M_PI * k * k);
    double p = b - (a * a) / 3.0;
    double q = (2.0 * a * a * a) / 27.0 - (a * b) / 3.0 + c;
    double D = (q * q) / 4.0 + (p * p * p) / 27.0;
    double y;

    if (D >= 0)
    {
        double sqrt_D = std::sqrt(D);
        double u = std::cbrt(-q / 2.0 + sqrt_D);
        double v = std::cbrt(-q / 2.0 - sqrt_D);
        y = u + v;
    }
    else
    {
        double r_ = std::sqrt(-p / 3.0);
        double theta = std::acos(std::clamp(-q / (2.0 * r_ * r_ * r_), -1.0, 1.0));
        y = 2.0 * r_ * std::cos(theta / 3.0);
    }
    double h = y - a / 3.0;
    h = std::round(h * 10.0) / 10.0;

    return h;
}

int main()
{
    float r, R, H, V, h, x;
    std::cin >> r >> R >> H >> V;
    std::cin.ignore();
    std::cerr << r << " " << R << " " << H << " " << V << std::endl;

    std::cout << std::fixed << std::setprecision(1) << compute_h(r, R, H, V) << std::endl;
}