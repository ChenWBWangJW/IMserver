GCC ?= gcc
CCMODE = PROGRAM
INCLUDES = -I/usr/include/mysql
CFLAGS = -Wall $(MACRO)
TARGET = test
SRCS := $(wildcard *.c)
LIBS = -L/usr/lib64/mysql -lmysqlclient

ifeq ($(CCMODE), PROGRAM)
$(TARGET): $(LINKS) $(SRCS)
	$(GCC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(SRCS) $(LIBS)
	@chmod +x $(TARGET)
	@echo make $(TARGET) ok.

clean:
	rm -rf $(TARGET)
endif
