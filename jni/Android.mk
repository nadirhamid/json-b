LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := jsonb
LOCAL_SRC_FILES := jsonb.c java.c

include $(BUILD_SHARED_LIBRARY)
# You can find more information about Android.mk at https://developer.android.com/ndk/guides/android_mk.html.

