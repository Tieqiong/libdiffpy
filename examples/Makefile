# Compiler and flags
CXX       := c++
CXXFLAGS  := -std=c++11 -I$(CONDA_PREFIX)/include
LDFLAGS   := -L$(CONDA_PREFIX)/lib -Wl,-rpath,$(CONDA_PREFIX)/lib
LDLIBS    := -ldiffpy -lObjCryst

# Source and target
SRC       := testlib.cpp
TARGET    := a.out

# Default target
all: $(TARGET)

# Link the target executable
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) $(LDFLAGS) $(LDLIBS) -o $(TARGET)

# Clean up build artifacts
clean:
	$(RM) $(TARGET)

.PHONY: all clean
