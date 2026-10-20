LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := antifraud_bypass

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/.. \
    $(LOCAL_PATH)/dobby/include \
    $(LOCAL_PATH)/dobby/source \
    $(LOCAL_PATH)/dobby/src

LOCAL_CFLAGS := -I$(LOCAL_PATH)/.. -I$(LOCAL_PATH)/dobby/include -I$(LOCAL_PATH)/dobby/source -I$(LOCAL_PATH)/dobby/src
LOCAL_CFLAGS += -DDOBBY_DEBUG -DLOG_TAG=\"Dobby\"

LOCAL_SRC_FILES := \
    ../module.cpp \
    dobby/source/dobby.c

LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)