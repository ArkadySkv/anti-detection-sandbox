#include <jni.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include "zygisk.hpp"
#include "dobby.h"

int (*orig_openat)(int dirfd, const char *pathname, int flags, mode_t mode);

int my_openat(int dirfd, const char *pathname, int flags, mode_t mode) {
    if (pathname != nullptr) {
        // Защита от детекта Frida/Xposed в картах памяти на Snapdragon
        if (strstr(pathname, "/proc/self/maps") != nullptr) {
            // Подменяем путь на чистый муляж, загруженный в систему
            return orig_openat(dirfd, "/data/local/tmp/clean_maps", flags, mode);
        }
        if (strstr(pathname, "/proc/net/unix") != nullptr) {
            return orig_openat(dirfd, "/data/local/tmp/clean_unix", flags, mode);
        }
    }
    return orig_openat(dirfd, pathname, flags, mode);
}

class AntiFraudModule : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }
    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        // Устанавливаем хук Dobby прямо перед запуском целевого приложения
        void *openat_addr = DobbySymbolResolver(nullptr, "openat");
        if (openat_addr != nullptr) {
            DobbyHook(openat_addr, (dobby_dummy_func_t)my_openat, (dobby_dummy_func_t *)&orig_openat);
        }
    }
private:
    zygisk::Api *api;
    JNIEnv *env;
};

REGISTER_ZYGISK_MODULE(AntiFraudModule)
