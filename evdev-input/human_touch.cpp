#include <iostream>
#include <cmath>
#include <random>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <string>

struct Point {
    double x;
    double y;
};

double get_random_noise(double min, double max) {
    static std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> dis(min, max);
    return dis(gen);
}

Point calculate_bezier_point(Point p0, Point p1, Point p2, Point p3, double t) {
    Point p;
    p.x = std::pow(1 - t, 3) * p0.x + 3 * std::pow(1 - t, 2) * t * p1.x + 3 * (1 - t) * std::pow(t, 2) * p2.x + std::pow(t, 3) * p3.x;
    p.y = std::pow(1 - t, 3) * p0.y + 3 * std::pow(1 - t, 2) * t * p1.y + 3 * (1 - t) * std::pow(t, 2) * p2.y + std::pow(t, 3) * p3.y;
    
    // Добавляем человеческий шум (джиттер)
    p.x += get_random_noise(-1.5, 1.5);
    p.y += get_random_noise(-1.5, 1.5);
    return p;
}

int main() {
    Point start = {300.0, 1500.0}; 
    Point end = {350.0, 400.0};    

    Point p1 = { start.x + get_random_noise(-50, 50), start.y + get_random_noise(20, 100) };
    Point p2 = { end.x + get_random_noise(-50, 50), end.y + get_random_noise(-100, -20) };

    std::cout << "[+] Запуск No-Root эмуляции ПФ через подсистему ввода Android..." << std::endl;

    int steps = 25; // Уменьшаем шаги для оптимизации передачи через shell
    for (int i = 0; i <= steps; ++i) {
        double t = static_cast<double>(i) / steps;
        Point current = calculate_bezier_point(start, p1, p2, end, t);

        // Используем внутренний инжектор Android, доступный через ADB без Root
        std::string cmd = "input touchscreen swipe " + 
                          std::to_string(static_cast<int>(current.x)) + " " + 
                          std::to_string(static_cast<int>(current.y)) + " " +
                          std::to_string(static_cast<int>(current.x)) + " " + 
                          std::to_string(static_cast<int>(current.y)) + " 10";
        
        std::system(cmd.c_str());
        
        double speed_noise = 10.0 + get_random_noise(-2.0, 2.0);
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(speed_noise)));
    }

    std::cout << "[+] Эмуляция завершена!" << std::endl;
    return 0;
}
