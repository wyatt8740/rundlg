# Wyatt Ward, 2021
# Makefile for my FLTK run dialogue.

FLTK_FLAGS=`fltk-config --cxxflags`
#FLTK_FLAGS=`fltk-config --cflags`

# for STATIC linking with fltk, do:
FLTK_LIBS=`fltk-config --ldstaticflags`

# for SHARED linking with libfltk.so, do:
# FLTK_LIBS=`fltk-config --ldflags`

LDLIBS=$(FLTK_LIBS)

#CXXFLAGS=-g -Wall $(FLTK_FLAGS)
CXXFLAGS=-O2 -Wall $(FLTK_FLAGS)

# Additional settings:
# define these if your make implementation doesn't include definitions for
# compilers (CC, CXX, etc.), or if you want to specify one.
# CC=gcc
# CXX=g++
# LD=ld
# for custom LDFLAGS to be appended:
# LDFLAGS=-lwhatever

.PHONY: all

all: rundlg

rundlg: rundlg.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS) $(LDFLAGS)

.PHONY: clean

clean:
	rm -f rundlg *.o
