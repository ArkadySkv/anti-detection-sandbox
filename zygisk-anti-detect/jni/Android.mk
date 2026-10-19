LOCAL_PATH := $(call my-dir)

# Задаем конфигурацию исходников Dobby для сборщика ndk-build
DOBBY_SOURCE_DIR := $(LOCAL_PATH)/dobby

# Включаем заголовочные файлы локальной jni папки и инклуды Dobby
LOCAL_C_INCLUDES := $(LOCAL_PATH)/.. $(LOCAL_PATH)/dobby/include

# Собираем основной Zygisk-модуль
include $(CLEAR_VARS)
LOCAL_MODULE := antifraud_bypass

# ИСПРАВЛЕНО: Указываем правильный относительный путь к файлу module.cpp
LOCAL_SRC_FILES := ../module.cpp

# Добавляем все необходимые исходные файлы Dobby для архитектуры ARM64
LOCAL_SRC_FILES += \
    dobby/source/dobby.cc \
    dobby/source/Interceptor.cc \
    dobby/source/TrampolineBridge/ClosureTrampolineBridge/ClosureTrampolineBridge.cc \
    dobby/source/InstructionRelocation/Arm64/InstructionRelocationArm64.cc \
    dobby/source/PlatformUnifiedInterface/ExecMemory/ClearCache-linux.cc \
    dobby/source/PlatformUnifiedInterface/ExecMemory/PlatformExecMemory-linux.cc

# Задаем флаги компиляции для сборки Dobby внутри проекта
LOCAL_CFLAGS := -DDOBBY_DEBUG -DLOG_TAG=\"Dobby\"
LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)
