LOCAL_PATH := $(call my-dir)

# Собираем основной Zygisk-модуль
include $(CLEAR_VARS)
LOCAL_MODULE := antifraud_bypass

# ВАЖНО: Объявляем пути поиска инклудов СТРОГО здесь, чтобы include $(CLEAR_VARS) их не стирал
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/.. \
    $(LOCAL_PATH)/dobby/include \
    $(LOCAL_PATH)/dobby/source

# Принудительно передаем компилятору пути поиска через флаги (-I), чтобы обойти любые баги ndk-build
LOCAL_CFLAGS := -I$(LOCAL_PATH)/.. -I$(LOCAL_PATH)/dobby/include -I$(LOCAL_PATH)/dobby/source

# Дополнительные флаги для правильной сборки исходников Dobby
LOCAL_CFLAGS += -DDOBBY_DEBUG -DLOG_TAG=\"Dobby\"

# Указываем путь к вашему файлу module.cpp
LOCAL_SRC_FILES := ../module.cpp

# Добавляем все необходимые исходные файлы Dobby для архитектуры ARM64
LOCAL_SRC_FILES += \
    dobby/source/dobby.cc \
    dobby/source/Interceptor.cc \
    dobby/source/TrampolineBridge/ClosureTrampolineBridge/ClosureTrampolineBridge.cc \
    dobby/source/InstructionRelocation/Arm64/InstructionRelocationArm64.cc \
    dobby/source/PlatformUnifiedInterface/ExecMemory/ClearCache-linux.cc \
    dobby/source/PlatformUnifiedInterface/ExecMemory/PlatformExecMemory-linux.cc

LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)
