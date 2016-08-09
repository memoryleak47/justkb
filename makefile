both: compile run

compile:
	g++ src/*.cpp -o exe
run:
	sudo ./exe
.PHONY: both compile run
