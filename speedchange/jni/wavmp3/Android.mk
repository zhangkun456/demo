LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libtxle

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/lib/ 

#LOCAL_EXPORT_C_INCLUDES := \
#	$(LOCAL_PATH)/soundtouch/include/


LOCAL_SRC_FILES := \
	wavmp3.c \
	lib/*.c 
	
LOCAL_LDLIBS    := -lm -llog
include $(BUILD_SHARED_LIBRARY)
