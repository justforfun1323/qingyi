CXX = g++ -std=c++11 

TARGET = target

ROOT_PATH = ./../../

CUR_DIR = .

INCLUDE_PATH = -I./\
	       -I$(ROOT_PATH) \

BASE_OBJS := $(wildcard $(CUR_DIR)/*.cc)

OBJS = $(patsubst %.cc, %.o, $(BASE_OBJS))

LIBS = -lpthread

.PHONY:all clean
all: $(TARGET)
	@echo "successful go go go"
$(TARGET): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(INCLUDE_PATH) $(LIBS)
$(OBJS): %.o : %.cc
	$(CXX) -c $< -o $@ $(INCLUDE_PATH)
clean:
	rm -rf $(TARGET) *.o



