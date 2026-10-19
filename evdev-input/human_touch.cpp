#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <cmath>
#include <random>
#include <chrono>
#include <thread>
#include <string>

struct Point {
    double x;
    double y;
};

// Функция генерации физического шума дрожания рук
double get_random_noise(double min, double max) {
    static std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> dis(min, max);
    return dis(gen);
}

// Расчет траектории Безье с интеграцией джиттера
Point calculate_bezier_point(Point p0, Point p1, Point p2, Point p3, double t) {
    Point p;
    p.x = std::pow(1 - t, 3) * p0.x + 3 * std::pow(1 - t, 2) * t * p1.x + 3 * (1 - t) * std::pow(t, 2) * p2.x + std::pow(t, 3) * p3.x;
    p.y = std::pow(1 - t, 3) * p0.y + 3 * std::pow(1 - t, 2) * t * p1.y + 3 * (1 - t) * std::pow(t, 2) * p2.y + std::pow(t, 3) * p3.y;
    
    p.x += get_random_noise(-1.2, 1.2);
    p.y += get_random_noise(-1.2, 1.2);
    
    return p;
}

// Прямая запись структуры input_event в ядро Linux Evdev
void send_event(int fd, __u16 type, __u16 code, __s32 value) {
    struct input_event ev;
    ev.type = type;
    ev.code = code;
    ev.value = value;
    ev.time.tv_sec = 0;
    ev.time.tv_usec = 0;
    write(fd, &ev, sizeof(struct input_event));
}

void emulate_human_swipe(const char* device_path, Point start, Point end) {
    int fd = open(device_path, O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
        std::cerr << "Не удалось открыть evdev-устройство " << device_path << ". Нужен su/root!" << std::endl;
        return;
    }

    // Случайные опорные точки для нелинейности движения
    Point p1 = { start.x + get_random_noise(-60, 60), start.y + get_random_noise(30, 150) };
    Point p2 = { end.x + get_random_noise(-60, 60), end.y + get_random_noise(-150, -30) };

    std::cout << "Инжекция нативного Touch Down..." << std::endl;
    send_event(fd, EV_ABS, ABS_MT_TRACKING_ID, 1);
    send_event(fd, EV_ABS, ABS_MT_POSITION_X, start.x);
    send_event(fd, EV_ABS, ABS_MT_POSITION_Y, start.y);
    send_event(fd, EV_SYN, SYN_REPORT, 0);

    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(get_random_noise(30, 60))));

    int steps = 40;
    for (int i = 0; i <= steps; ++i) {
        double t = static_cast<double>(i) / steps;
        Point current = calculate_bezier_point(start, p1, p2, end, t);

        send_event(fd, EV_ABS, ABS_MT_POSITION_X, static_cast<int>(current.x));
        send_event(fd, EV_ABS, ABS_MT_POSITION_Y, static_cast<int>(current.y));
        send_event(fd, EV_SYN, SYN_REPORT, 0);

        // Динамическая задержка — человек ведет палец неравномерно
        double speed_noise = 7.0 + std::sin(t * M_PI) * 4.0 + get_random_noise(-1.5, 1.5);
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(speed_noise)));
    }

    std::cout << "[+] Инжекция нативного Touch Up..." << std::endl;
    send_event(fd, EV_ABS, ABS_MT_TRACKING_ID, -1);
    send_event(fd, EV_SYN, SYN_REPORT, 0);

    close(fd);
}

int main(int argc, char* argv[]) {
    // Индекс по умолчанию для тачскрина
    std::string touch_device = "/dev/input/event4"; 

    // Позволяет передавать индекс аргументом, например: ./human_touch 4
    if (argc > 1) {
        touch_device = "/dev/input/event" + std::string(argv[1]);
    }

    std::cout << "[*] Нацелено на evdev: " << touch_device << std::endl;
    
    // Стандартные экранные координаты для Redmi 10C
    Point start_point = {400.0, 1400.0}; 
    Point end_point = {420.0, 300.0};    

    emulate_human_swipe(touch_device.c_str(), start_point, end_point);
    return 0;
}
