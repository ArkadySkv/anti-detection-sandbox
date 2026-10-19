LOCAL_PATH := $(call my-dir)

# Явно задаем пути к инклудам через LOCAL_PATH
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/.. \
    $(LOCAL_PATH)/dobby/include \
    $(LOCAL_PATH)/dobby/source

# Собираем основной Zygisk-модуль
include $(CLEAR_VARS)
LOCAL_MODULE := antifraud_bypass

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

# Флаги компиляции для сборки исходников Dobby внутри проекта
LOCAL_CFLAGS := -DDOBBY_DEBUG -DLOG_TAG=\"Dobby\"
LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)
