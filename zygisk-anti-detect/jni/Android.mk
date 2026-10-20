LOCAL_PATH := $(call my-dir)

DOBBY_SOURCE_DIR := $(LOCAL_PATH)/dobby
include $(DOBBY_SOURCE_DIR)/Android.mk

include $(CLEAR_VARS)
LOCAL_MODULE := antifraud_bypass

LOCAL_SRC_FILES := ../module.cpp

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/.. \
    $(DOBBY_SOURCE_DIR)/include

LOCAL_STATIC_LIBRARIES := dobby_static
LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)
