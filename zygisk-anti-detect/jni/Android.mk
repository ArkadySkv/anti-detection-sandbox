LOCAL_PATH := $(call my-dir)

# 1. Объявляем предварительно собранную статическую библиотеку Dobby из правильной папки репозитория LSPosed
include $(CLEAR_VARS)
LOCAL_MODULE := dobby
LOCAL_SRC_FILES := ../dobby/builtin-plugin/android-prebuilt/arm64-v8a/libdobby.a
include $(PREBUILT_STATIC_LIBRARY)

# 2. Собираем основной Zygisk-модуль
include $(CLEAR_VARS)
LOCAL_MODULE := antifraud_bypass
LOCAL_SRC_FILES := ../module.cpp

# Указываем пути к заголовочным файлам относительно папки jni
LOCAL_C_INCLUDES := $(LOCAL_PATH)/.. $(LOCAL_PATH)/../dobby/include

LOCAL_STATIC_LIBRARIES := dobby
LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)
