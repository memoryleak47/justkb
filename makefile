both: compile run

compile:
	g++ src/*.cpp -std=c++14 -lX11 -o exe
run:
	gksudo ./exe
.PHONY: both compile run
