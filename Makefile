csrc = $(wildcard src/*.c)
ccsrc = $(wildcard src/*.cc)
obj = $(csrc:.c=.o) $(ccsrc:.cc=.o)
dep = $(obj:.o=.d)

bin = dicomview

#opt = -O3 -ffast-math
dbg = -g
def = -DUSE_GLUT
#inc =

CFLAGS = -pedantic -Wall $(opt) $(dbg) $(def) $(inc)
CXXFLAGS = $(CFLAGS)
LDFLAGS = $(libgl) -lm -limago

ifeq ($(shell uname -s), Darwin)
	libgl = -framework OpenGL -framework GLUT -lGLEW
else
	libgl = -lGL -lGLU -lglut -lGLEW
endif

$(bin): $(obj)
	$(CXX) -o $@ $(obj) $(LDFLAGS)

-include $(dep)

%.d: %.c
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

%.d: %.cc
	@$(CPP) $(CXXFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	rm -f $(obj) $(bin)

.PHONY: cleandep
cleandep:
	rm -f $(dep)

.PHONY: install
install:
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp $(bin) $(DESTDIR)$(PREFIX)/bin/$(bin)

.PHONY: uninstall
	rm -f $(bin) $(DESTDIR)$(PREFIX)/bin/$(bin)
