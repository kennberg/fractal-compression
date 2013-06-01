
OPT = 

OBJ = IFSTransform.o\
	Image.o\
	Decoder.o\
	Encoder.o\
	QuadTreeEncoder.o

all: fractal

IFSTransform.o: IFSTransform.h IFSTransform.cpp
	g++ $(OPT) -c IFSTransform.cpp

Image.o: Image.h Image.cpp
	g++ $(OPT) -c Image.cpp

Decoder.o: Decoder.h Decoder.cpp
	g++ $(OPT) -c Decoder.cpp

Encoder.o: Encoder.h Encoder.cpp
	g++ $(OPT) -c Encoder.cpp

QuadTreeEncoder.o: QuadTreeEncoder.h QuadTreeEncoder.cpp
	g++ $(OPT) -c QuadTreeEncoder.cpp

fractal: $(OBJ) main.cpp
	g++ $(OPT) -o fractal $(OBJ) main.cpp

clean:
	rm *.o
	rm fractal*


