SRCDIR := practices
OBJDIR := build
SUBDIR := $(OBJDIR)/$(SRCDIR)

SOURCES := $(wildcard $(SRCDIR)/*.cpp)
TARGETS := $(patsubst $(SRCDIR)/%.cpp,$(SUBDIR)/%,$(SOURCES))

CFLAGS = -std=c++20

# 得到所有 SOURCE 对应的 可执行文件
all: $(TARGETS)

$(SUBDIR)/%: $(SRCDIR)/%.cpp | $(SUBDIR)
	g++ $(CFLAGS) $< -o $@

$(SUBDIR):
	mkdir -p $@

clean:
	rm -rf $(OBJDIR)