CC	= g++
CFLAGS	= -std=c++14 -O2 -MMD -MP -DNDEBUG
LDFLAGS	= `pkg-config --libs opencv` -lraspicam -lraspicam_cv -lpthread
LIBS	=
INCLUDE	= `pkg-config --cflag opencv` -I ./include
SRC_DIR	= ./src
OBJ_DIR	= ./build
SOURCES	= $(shell ls $(SRC_DIR)/*.cpp)
OBJS	= $(subst $(SRC_DIR),$(OBJ_DIR), $(SOURCES:.cpp=.o))
TARGET	= MazeRunner
DEPENDS	= $(OBJS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJS) $(LIBS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp 
	@if [ ! -d $(OBJ_DIR) ]; \
	then echo "mkdir -p $(OBJ_DIR)"; mkdir -p $(OBJ_DIR); \
	fi
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

clean:
	$(RM) $(OBJS) $(TARGET) $(DEPENDS)

-include $(DEPENDS)

.PHONY: all clean