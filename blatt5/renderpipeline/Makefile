HEADERS = cmdline.h scene.h raster.h
SOURCES = main.cpp cmdline.cpp scene.cpp raster.cpp

CXXFLAGS = -ggdb3 -O0
# CXXFLAGS = -O3

renderpipeline: $(SOURCES) $(HEADERS)
	# g++ $(CXXFLAGS) -std=c++14 $(SOURCES) -lpng -o $@
	g++ -ggdb3 -O0 -I/opt/homebrew/include -L/opt/homebrew/lib -std=c++14 main.cpp cmdline.cpp scene.cpp raster.cpp -lpng -largp -o renderpipeline

.PHONY: dist clean

dist: renderpipeline.tar.bz2

clean:
	rm -f renderpipeline

renderpipeline.tar.bz2: $(SOURCES) $(HEADERS)
	mkdir renderpipeline
	cp $(SOURCES) $(HEADERS) bunny.data rot.py Makefile renderpipeline
	tar cfj $@ renderpipeline
