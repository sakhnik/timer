bin_PROGRAMS = timer

timer_SOURCES = main.c \
		digit.c digit.h \
		number.c number.h \
		clock.c clock.h

AM_CPPFLAGS = @GTK_CFLAGS@ @PTHREAD_CFLAGS@
LDADD = @GTK_LIBS@ @PTHREAD_CFLAGS@

EXTRA_DIST = beep0.wav beep1.wav
