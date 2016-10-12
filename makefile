both: compile run

compile:
	g++ src/*.cpp -std=c++14 -lX11 -o exe
run:
	sudo ./exe
	#@set-keymap ger && set-keymap std
.PHONY: both compile run
