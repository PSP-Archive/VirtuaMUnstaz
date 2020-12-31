TARGET = vmudemo
OBJS = main.o scroller.o vms/cpu.o vms/psp.o

INCDIR =
CFLAGS = -G0 -Wall -DHAVE_SYS_TIME_H=1 -DHAVE_FCNTL_H=1 -DBSD_STYLE_GETTIMEOFDAY=1
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =
LIBS= -lpspgu

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = VirtuaMUnstaz VMS Demo
PSP_EBOOT_ICON = ICON0.PNG

PSPSDK = $(shell psp-config --pspsdk-path)
PSPBIN = $(PSPSDK)/../bin
CFLAGS += `$(PSPBIN)/sdl-config --cflags`
LIBS += `$(PSPBIN)/sdl-config --libs`
include  ${PSPSDK}/lib/build.mak
