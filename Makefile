src=main.cpp
run: ${src}
	g++ ${src} -lncursesw && ./a.out

publish: ${src}
	g++ main.cpp -o ConsoleOS -lncursesw