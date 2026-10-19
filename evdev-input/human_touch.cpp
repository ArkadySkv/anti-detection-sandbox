#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <cmath>
#include <random>
#include <chrono>
#include <thread>

// Структура для координат
struct Point {
    double x;
    double y;
};

// Функция генерации случайного числа в диапазоне (для джиттера и таймингов)
double get_random_noise(double min, double max) {
    static std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> dis(min, max);
    return dis(gen);
}

// Вычисление точки на кубической кривой Безье с добавлением джиттера (микро-колебаний)
Point calculate_bezier_point(Point p0, Point p1, Point p2, Point p3, double t) {
    Point p;
    // Классическая формула кубического Безье
    p.x = std::pow(1 - t, 3) * p0.x + 3 * std::pow(1 - t, 2) * t * p1.x + 3 * (1 - t) * std::pow(t, 2) * p2.x + std::pow(t, 3) * p3.x;
    p.y = std::pow(1 - t, 3) * p0.y + 3 * std::pow(1 - t, 2) * t * p1.y + 3 * (1 - t) * std::pow(t, 2) * p2.y + std::pow(t, 3) * p3.y;
    
    // Добавляем микро-джиттер руки человека (шум в пределах 0.5-1.5 пикселя)
    p.x += get_random_noise(-1.2, 1.2);
    p.y += get_random_noise(-1.2, 1.2);
    
    return p;
}

// Отправка события в evdev
void send_event(int fd, __u16 type, __u16 code, __s32 value) {
    struct input_event ev;
    ev.type = type;
    ev.code = code;
    ev.value = value;
    // Тайминги оставляем нулевыми, ядро Android заполнит их автоматически при получении
    ev.time.tv_sec = 0;
    ev.time.tv_usec = 0;
    write(fd, &ev, sizeof(struct input_event));
}

// Эмуляция человеческого свайпа по экрану
void emulate_human_swipe(const char* device_path, Point start, Point end) {
    // Открываем тачскрин в режиме записи. Требуются root-права на девайсе!
    int fd = open(device_path, O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
        std::cerr << "[-] Ошибка: Не удалось открыть evdev девайс " << device_path << " (Нужен root)" << std::endl;
        return;
    }

    // Генерируем фейковые опорные точки для кривой Безье, чтобы траектория была нелинейной
    Point p1 = { start.x + get_random_noise(-50, 50), start.y + get_random_noise(20, 100) };
    Point p2 = { end.x + get_random_noise(-50, 50), end.y + get_random_noise(-100, -20) };

    std::cout << "[+] Начинаем свайп через evdev..." << std::endl;

    // 1. Нажатие (Touch Down)
    send_event(fd, EV_ABS, ABS_MT_TRACKING_ID, 1); // ID пальца (мультитач)
    send_event(fd, EV_ABS, ABS_MT_POSITION_X, start.x);
    send_event(fd, EV_ABS, ABS_MT_POSITION_Y, start.y);
    send_event(fd, EV_SYN, SYN_REPORT, 0);

    // Удержание пальца в начале (микро-задержка нажатия)
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(get_random_noise(40, 70))));

    // 2. Движение по кривой
    int steps = 45; // Количество шагов отправки координат
    for (int i = 0; i <= steps; ++i) {
        double t = static_cast<double>(i) / steps;
        Point current = calculate_bezier_point(start, p1, p2, end, t);

        send_event(fd, EV_ABS, ABS_MT_POSITION_X, static_cast<int>(current.x));
        send_event(fd, EV_ABS, ABS_MT_POSITION_Y, static_cast<int>(current.y));
        send_event(fd, EV_SYN, SYN_REPORT, 0);

        // Динамический шаг времени (человек ведет палец неравномерно: ускоряется и замедляется)
        double speed_noise = 8.0 + std::sin(t * M_PI) * 5.0 + get_random_noise(-2.0, 2.0);
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(speed_noise)));
    }

    // 3. Отпускание (Touch Up)
    send_event(fd, EV_ABS, ABS_MT_TRACKING_ID, -1); // -1 означает, что палец убран
    send_event(fd, EV_SYN, SYN_REPORT, 0);

    close(fd);
    std::cout << "[+] Свайп успешно завершен." << std::endl;
}

int main() {
    // В Android тачскрин обычно находится на /dev/input/event1, event2 или event3.
    // Узнать точный можно командой: adb shell getevent -p
    const char* touch_device = "/dev/input/event2"; 
    
    Point start_point = {300.0, 1500.0}; // Начало свайпа (низ экрана)
    Point end_point = {350.0, 400.0};    // Конец свайпа (верх экрана)

    emulate_human_swipe(touch_device, start_point, end_point);
    return 0;
}
