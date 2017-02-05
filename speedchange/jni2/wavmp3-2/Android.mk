LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libtxle

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/wavmp3/libavcodec/ \
	$(LOCAL_PATH)/wavmp3/libavdevice/ \
	$(LOCAL_PATH)/wavmp3/libavfilter/ \
	$(LOCAL_PATH)/wavmp3/libavformat/ \
	$(LOCAL_PATH)/wavmp3/libavutil/ \
	$(LOCAL_PATH)/wavmp3/libswscale/ 

#LOCAL_EXPORT_C_INCLUDES := \
#	$(LOCAL_PATH)/soundtouch/include/


LOCAL_SRC_FILES := \
	wavmp3.c \
	libavcodec/*.c \
	libavdevice/*.c \
	libavfilter/*.c \
	libavformat/*.c \
	libavutil/*.c \
	libswscale/*.c
	
LOCAL_LDLIBS    := -lm -llog
include $(BUILD_SHARED_LIBRARY)
