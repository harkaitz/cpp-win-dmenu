.POSIX: # POSIX Makefile, use make,gmake,pdpmake,bmake
.SUFFIXES:
.PHONY: all clean install check

PROJECT  =dmenu
VERSION  =1.0.0

PREFIX     =/usr/local
TOOLCHAINS =x86_64-w64-mingw32
TPREFIX    =x86_64-w64-mingw32-

CXX            =$(TPREFIX)g++
CXXFLAGS       =-Wall -g3
CXXFLAGS_DMENU =-static -static-libgcc -static-libstdc++ -DUNICODE 
LIBS           =-lgdi32
BUILDDIR      ?=.

CXXFLAGS_DMENU += -DPROGRAM_NAME=L\"$(PROJECT)\"
CXXFLAGS_DMENU += -DPROGRAM_VERSION=\"dmenu-win-$(VERSION)\"

PROGS      =$(BUILDDIR)/dmenu.exe

all: $(PROGS)
clean:
	rm -f $(PROGS)
install:
	install -d $(DESTDIR)$(PREFIX)/bin
	install -c -m 755 $(PROGS) $(DESTDIR)$(PREFIX)/bin
check:

$(BUILDDIR)/dmenu.exe: dmenu.cpp config.h
	$(CXX) -o $@ dmenu.cpp $(CXXFLAGS) $(CXXFLAGS_DMENU) $(LDFLAGS) $(LIBS)

release:
	hrelease -t "$(TOOLCHAINS)" -N $(PROJECT) -R $(VERSION) -o $(BUILDDIR)/Release
	gh release create v$(VERSION) $$(cat $(BUILDDIR)/Release)
