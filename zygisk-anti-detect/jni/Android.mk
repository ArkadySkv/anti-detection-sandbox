LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := dobby
LOCAL_SRC_FILES := libdobby.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := antifraud_bypass

LOCAL_SRC_FILES := ../module.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/.. $(LOCAL_PATH)/dobby_includes

LOCAL_STATIC_LIBRARIES := dobby
LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)