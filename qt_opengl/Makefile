# the $< is the first item in the dependencies list
# $@ and $^ are the left and right sides of the :, respectively
# $* is the left side of the :, without suffix

ODIR = obj
dummy_build_folder := $(shell mkdir -p $(ODIR))
   
CC = g++
CFLAGS = -pipe -O2 -Wall -Wextra
HEADERS = GLWidget.h 
_OBJ = GLWidget.o main.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
IDIR = -I. -I/usr/include/qt4
LDIR = -L. -lm -lQtOpenGL -lQtGui -lQtCore -lGLU -lGL -lX11 -lXext
OUT = main
	
all: $(OUT) 
$(OUT): $(OBJ)
	$(CC) -o $@ $^ $(LDIR)
	
$(ODIR)/%.o: %.cpp $(HEADERS) 
	$(CC) -c -o $@ $< $(CFLAGS) $(IDIR)
	
clean:	
	rm -rf $(ODIR)/*.o *.txt *~ $(OUT)
