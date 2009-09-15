bin_PROGRAMS = timer

timer_SOURCES = main.c \
		digit.c digit.h \
		number.c number.h \
		clock.c clock.h

timer_CPPFLAGS = @GTK_CFLAGS@ @PTHREAD_CFLAGS@
timer_LDADD = @GTK_LIBS@ @PTHREAD_LIBS@

EXTRA_DIST = beep0.wav beep1.wav
