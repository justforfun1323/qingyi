CXX = g++ -std=c++11 

TARGET = target

ROOT_PATH = ./../..

QINGYI_PATH = $(ROOT_PATH)/qingyi

CUR_DIR = .

INCLUDE_PATH = -I./\
	       -I$(ROOT_PATH)\

UTIL_PATH = $(QINGYI_PATH)/util

BASE_OBJS := $(wildcard $(UTIL_PATH)/*.cc)
BASE_OBJS += $(wildcard $(CUR_DIR)/*.cc)

OBJS = $(patsubst %.cc, %.o, $(BASE_OBJS))

LIBS = -lpthread

.PHONY:all clean
all: $(TARGET)
	@echo "successful go go go"
$(TARGET): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(INCLUDE_PATH) $(LIBS)
$(OBJS): %.o : %.cc
	@echo $(UTIL_PATH)
	$(CXX) -c $< -o $@ $(INCLUDE_PATH)
clean:
	rm -rf $(TARGET) $(OBJS)



