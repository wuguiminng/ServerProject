LOCAL_PATH := $(call my-dir)  

include $(CLEAR_VARS)  

LOCAL_MODULE := main

LOCAL_SRC_FILES :=  \
	src/main.cpp     \
	src/cjson/cJSON.c \
	src/ServerAes.cpp  \
	src/ServerRecv.cpp  \
	src/ServerSend.cpp   \
	src/ServerFile.cpp    \
	src/ServerTools.cpp    \
	src/ServerSocket.cpp    \
	src/tinyxml/tinystr.cpp  \
	src/tinyxml/tinyxml.cpp   \
	src/tinyxml/tinyxmlerror.cpp\
	src/tinyxml/tinyxmlparser.cpp

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/include\
    $(LOCAL_PATH)/include/curl \
	$(LOCAL_PATH)/include/cjson \
	$(LOCAL_PATH)/include/tinyxml \
	$(LOCAL_PATH)/include/openssl

LOCAL_LDLIBS:=-L$(SYSROOT)/usr/lib -llog 
LOCAL_LDLIBS += -L ./comnlib -lcurl -lcrypto
LOCAL_CFLAGS += -Wall -Werror
 
include $(BUILD_EXECUTABLE) 
