OBJS 	= main.o visuals.o
SOURCE	= main.cpp visuals.cpp
HEADER  = visuals.h
OUT  	= a.out
CC		= g++
FLAGS 	= -c -g 
GLUT	= -lglut -lGLU -lGL
# -g option enables debugging mode 
# -c flag generates object code for separate files

all: $(OBJS)
	$(CC) $(OBJS) -o $(OUT) $(GLUT)

# create/compile the individual files >>separately<< 
main.o: main.cpp visuals.h
	$(CC) $(FLAGS) main.cpp

visuals.o: visuals.cpp visuals.h
	$(CC) $(FLAGS) visuals.cpp

# clean house
clean:
	rm -f $(OBJS) $(OUT)
