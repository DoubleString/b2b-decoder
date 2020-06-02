CC = gcc  
SO = -lpthread  # -lgcc_s
SRCS = $(wildcard *.c)
OBJS = $(patsubst %c, %o, $(SRCS)) 
SH = -shared
SA = -static 
TARGET = b2b-decoder
.PHONY:all clean 
all: $(TARGET) 
#-shared
$(TARGET): $(OBJS) 
	$(CC)   -o  $@ $^  -lm  $(SO)
#	$(CC) $(SH)  -o   $@ $^  -lm  $(SO)
	rm $(OBJS)
%.o:%.c 
	$(CC) -fpic -g  -c -DTRACE  $^ 
clean: 
	rm -f  $(OBJS)   $(TARGET)

