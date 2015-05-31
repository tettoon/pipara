CXX      := g++
CXXFLAGS := -I. -I../native -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -L../native -lpipara

CLASSDIR := ../java/build/classes/main

.PHONY: all
all: libpiparajni.so

javah: $(CLASSDIR)/tettoon/pipara/PiParaJni.class
	javah -cp $(CLASSDIR) tettoon.pipara.PiParaJni

libpiparajni.so: PiParaJni.cpp javah
	$(CXX) $(CXXFLAGS) -fPIC -shared -o $@ $<

.PHONY: clean
clean:
	$(RM) libpiparajni.so
	$(RM) tettoon_pipara_PiParaJni.h

