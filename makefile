
all:
	g++ src/main.cc src/utils.cc src/scan.cc -I./src/include -o build/gitbully

clean:
	rm build/*
