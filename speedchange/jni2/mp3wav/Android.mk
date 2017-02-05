LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libtxld

#LOCAL_C_INCLUDES := \
#	$(LOCAL_PATH)/mp3wav/include/ \
#	$(LOCAL_PATH)/mp3wav/source/SoundTouch/

#LOCAL_EXPORT_C_INCLUDES := \
#	$(LOCAL_PATH)/soundtouch/include/

LOCAL_SRC_FILES := \
	mp3wav.c \
	audio.c \
	mpglib/common.c \
	mpglib/dct64_i386.c \
	mpglib/decode_i386.c \
	mpglib/layer2.c \
	mpglib/layer3.c \
	mpglib/mpg123.c \
	mpglib/tabinit.c 
LOCAL_LDLIBS    := -lm -llog
include $(BUILD_SHARED_LIBRARY)
