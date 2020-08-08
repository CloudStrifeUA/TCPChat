SRC_DIR := src
OBJ_DIR := obj

SRC := $(wildcard $(SRC_DIR)/*.cpp)
CLIENT_SRC := $(wildcard $(SRC_DIR)/client*.cpp)
SERVER_SRC := $(wildcard $(SRC_DIR)/server*.cpp)
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
CLIENT_OBJ := $(CLIENT_SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
SERVER_OBJ := $(SERVER_SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
SOCKET_OBJ := $(OBJ_DIR)/socket_wrapper.o

CPPFLAGS := -Iinclude -MMD -MP
CFLAGS   := -Wall -std=c++17
LDFLAGS  := 
LDLIBS   := -lpthread

.PHONY: all clean client server

all: socket_wrapper client server

socket_wrapper: $(SOCKET_OBJ)

client: $(CLIENT_OBJ) 
	$(CXX) $(LDFLAGS) $^ $(SOCKET_OBJ) $(LDLIBS) -o $@

server: $(SERVER_OBJ)
	$(CXX) $(LDFLAGS) $^ $(SOCKET_OBJ) $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(OBJ_DIR)

-include $(OBJ:.o=.d)