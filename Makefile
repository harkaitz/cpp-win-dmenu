.POSIX: # POSIX Makefile, use make,gmake,pdpmake,bmake
.SUFFIXES:
.PHONY: all clean install check

PROJECT  =dmenu
VERSION  =dmenu-win-1.0.0

PROGS      =dmenu.exe
PREFIX     =/usr/local
TOOLCHAINS =x86_64-w64-mingw32
TPREFIX    =x86_64-w64-mingw32-

CXX            =$(TPREFIX)g++
CXXFLAGS       =-Wall -g3
CXXFLAGS_DMENU =-static -static-libgcc -static-libstdc++ -DUNICODE 
LIBS           =-lgdi32

CXXFLAGS_DMENU += -DPROGRAM_NAME=L\"$(PROJECT)\"
CXXFLAGS_DMENU += -DPROGRAM_VERSION=\"$(VERSION)\"

all: $(PROGS)
clean:
	rm -f $(PROGS)
install:
	install -d $(DESTDIR)$(PREFIX)/bin
	install -c -m 755 $(PROGS) $(DESTDIR)$(PREFIX)/bin
check:

dmenu.exe: dmenu.cpp config.h
	$(CXX) -o $@ dmenu.cpp $(CXXFLAGS) $(CXXFLAGS_DMENU) $(LDFLAGS) $(LIBS)
