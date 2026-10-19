LOCAL_PATH := $(call my-dir)

# Собираем основной Zygisk-модуль
include $(CLEAR_VARS)
LOCAL_MODULE := antifraud_bypass

# Объявляем пути поиска инклудов
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/.. \
    $(LOCAL_PATH)/dobby/include \
    $(LOCAL_PATH)/dobby/source

# Принудительно передаем компилятору пути поиска через флаги
LOCAL_CFLAGS := -I$(LOCAL_PATH)/.. -I$(LOCAL_PATH)/dobby/include -I$(LOCAL_PATH)/dobby/source
LOCAL_CFLAGS += -DDOBBY_DEBUG -DLOG_TAG=\"Dobby\"

# Указываем путь к вашему файлу module.cpp
LOCAL_SRC_FILES := ../module.cpp

# ИСПРАВЛЕНО: Указаны точные расширения .c для исходников Dobby в репозитории LSPosed
LOCAL_SRC_FILES += \
    dobby/source/dobby.c \
    dobby/source/Interceptor.c \
    dobby/source/TrampolineBridge/ClosureTrampolineBridge/ClosureTrampolineBridge.c \
    dobby/source/InstructionRelocation/Arm64/InstructionRelocationArm64.c \
    dobby/source/PlatformUnifiedInterface/ExecMemory/ClearCache-linux.c \
    dobby/source/PlatformUnifiedInterface/ExecMemory/PlatformExecMemory-linux.cc

LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)
