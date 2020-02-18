BUILD=./build
DIST=./dist

build:
	mkdir -p $(BUILD)
	mkdir -p $(DIST)
	/opt/gbdk/bin/lcc -Wa-l -c -o $(BUILD)/snake.o src/snake.c
	/opt/gbdk/bin/lcc -Wa-l -c -o $(BUILD)/snake-map.o src/snake-map.c
	/opt/gbdk/bin/lcc -Wa-l -c -o $(BUILD)/snake-tiles.o src/snake-tiles.c
	/opt/gbdk/bin/lcc -Wa-l -c -o $(BUILD)/gameover.o src/gameover.c
	/opt/gbdk/bin/lcc -Wl-yt3 -Wl-yo4 -Wl-ya4 -o $(DIST)/snake.gb $(BUILD)/snake.o $(BUILD)/snake-map.o $(BUILD)/snake-tiles.o $(BUILD)/gameover.o 

clean:
	rm -rf $(BUILD)
	rm -f $(DIST)/snake.gb