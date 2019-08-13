CXX = g++ -std=c++11 

TARGET = target

ROOT_PATH = ./../..

QINGYI_PATH = $(ROOT_PATH)/qingyi

CUR_DIR = .

INCLUDE_PATH = -I./\
	       -I$(ROOT_PATH)\

UTIL_PATH = $(QINGYI_PATH)/util
NET_PATH = $(QINGYI_PATH)/net

BASE_OBJS := $(wildcard $(CUR_DIR)/*.cc)
BASE_OBJS += $(wildcard $(UTIL_PATH)/*.cc)
BASE_OBJS += $(wildcard $(NET_PATH)/*.cc)

OBJS = $(patsubst %.cc, %.o, $(BASE_OBJS))

LIBS = -lpthread -lboost_unit_test_framework

.PHONY:all clean
all: $(TARGET)
	@echo "successful go go go"
$(TARGET): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(INCLUDE_PATH) $(LIBS)
$(OBJS): %.o : %.cc
	$(CXX) -c $< -o $@ $(INCLUDE_PATH)
clean:
	rm -rf $(TARGET) $(OBJS)



