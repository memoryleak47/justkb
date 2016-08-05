both: compile run

compile:
	g++ src/*.cpp -lX11 -lXtst -o exe
run:
	./exe
.PHONY: both compile run
