#include <iostream>
#include <cmath>
#include <random>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <string>
#include <sstream>

struct Point {
    double x;
    double y;
};

// Генерация шума для имитации дрожания человеческой руки
double get_random_noise(double min, double max) {
    static std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> dis(min, max);
    return dis(gen);
}

// Расчет кривой Безье с добавлением джиттера
Point calculate_bezier_point(Point p0, Point p1, Point p2, Point p3, double t) {
    Point p;
    p.x = std::pow(1 - t, 3) * p0.x + 3 * std::pow(1 - t, 2) * t * p1.x + 3 * (1 - t) * std::pow(t, 2) * p2.x + std::pow(t, 3) * p3.x;
    p.y = std::pow(1 - t, 3) * p0.y + 3 * std::pow(1 - t, 2) * t * p1.y + 3 * (1 - t) * std::pow(t, 2) * p2.y + std::pow(t, 3) * p3.y;
    
    // Микро-шум, ломающий идеальную прямую линию бота
    p.x += get_random_noise(-1.5, 1.5);
    p.y += get_random_noise(-1.5, 1.5);
    return p;
}

int main() {
    Point start = {500.0, 1600.0}; // Начальная точка (середина-низ экрана)
    Point end = {520.0, 400.0};    // Конечная точка (середина-верх экрана)

    // Генерация случайных отклонений для построения дуги скролла
    Point p1 = { start.x + get_random_noise(-40, 40), start.y - get_random_noise(100, 300) };
    Point p2 = { end.x + get_random_noise(-40, 40), end.y + get_random_noise(100, 300) };

    std::cout << "[+] Генерация человеческого скролла Безье (No-Root)..." << std::endl;

    std::stringstream cmd_stream;
    // Формируем базовую команду: input touchscreen swipe <start_x> <start_y>
    cmd_stream << "input touchscreen swipe " << static_cast<int>(start.x) << " " << static_cast<int>(start.y);

    int steps = 10; // 10 промежуточных точек достаточно, чтобы Android построил плавный вектор движения
    for (int i = 1; i <= steps; ++i) {
        double t = static_cast<double>(i) / steps;
        Point current = calculate_bezier_point(start, p1, p2, end, t);
        
        // Дописываем координаты точек свайпа в строку
        cmd_stream << " " << static_cast<int>(current.x) << " " << static_cast<int>(current.y);
    }

    // Случайное время длительности свайпа (человек скроллит с разной скоростью: примерно 350-450 мс)
    int duration = static_cast<int>(get_random_noise(350, 450));
    cmd_stream << " " << duration;

    std::string final_cmd = cmd_stream.str();
    
    // Запускаем сформированную команду в систему Android
    std::system(final_cmd.c_str());

    std::cout << "[+] Свайп успешно выполнен за один проход!" << std::endl;
    return 0;
}
