CPP    = g++
WINDRES= windres
RM     = rm -f
OBJS   = anim_one.o \
         AppResource.res

LIBS   = -mwindows
CFLAGS =

.PHONY: anim_one.exe clean clean-after

all: anim_one.exe

clean:
	$(RM) $(OBJS) anim_one.exe

clean-after:
	$(RM) $(OBJS)

anim_one.exe: $(OBJS)
	$(CPP) -Wall -s -o $@ $(OBJS) $(LIBS)

anim_one.o: anim_one.cpp
	$(CPP) -Wall -s -c $< -o $@ $(CFLAGS)

AppResource.res: AppResource.rc
	$(WINDRES) -i AppResource.rc -J rc -o AppResource.res -O coff

