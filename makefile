both: compile run

compile:
	g++ src/*.cpp -o exe
run:
	sudo ./exe
	@set-keymap std
.PHONY: both compile run
