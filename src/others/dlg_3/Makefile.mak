CPP    = g++
WINDRES= windres
RM     = rm -f
OBJS   = dlg_three.o \
         AppResource.res

LIBS   = -mwindows
CFLAGS =

.PHONY: dlg_three.exe clean clean-after

all: dlg_three.exe

clean:
	$(RM) $(OBJS) dlg_three.exe

clean-after:
	$(RM) $(OBJS)

dlg_three.exe: $(OBJS)
	$(CPP) -Wall -s -o $@ $(OBJS) $(LIBS)

dlg_three.o: dlg_three.cpp resource.h
	$(CPP) -Wall -s -c $< -o $@ $(CFLAGS)

AppResource.res: AppResource.rc
	$(WINDRES) -i AppResource.rc -J rc -o AppResource.res -O coff

