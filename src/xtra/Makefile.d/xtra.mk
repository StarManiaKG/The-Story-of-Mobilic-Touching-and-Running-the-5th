#
# Makefile for feature flags.
# XTRA Edition!
#

CURRENT_OPTS:= 
CURRENT_SOURCES:=

undefine NOHW
HWRENDER=1

ifndef ANDROID
  CURRENT_OPTS+=$(opts)
else
  CURRENT_OPTS+=$(LOCAL_CFLAGS)
endif

CURRENT_SOURCES+=w_handle.c
CURRENT_SOURCES+=\
	$(call List,android/Sourcefile)\
	$(call List,xtra/Sourcefile)\

opts+=-DTOUCHINPUTS -DNATIVESCREENRES # -DHAVE_WHANDLE
opts+=-DHWRENDER -DHAVE_GLES -DHAVE_GLES2

ifndef NOHW
  $(info CHECKING FOR PROPER OPENGL...)
  ifeq (, $(findstring -DHWRENDER, $(CURRENT_OPTS)))
    ifeq (, $(findstring -DHAVE_GLES2, $(CURRENT_OPTS)))
      $(info BRO PLEASE)
      CURRENT_SOURCES+=hardware/r_gles/r_gles2.c sdl/ogl_es_sdl.c
    else ifeq (, $(findstring -DHAVE_GLES, $(CURRENT_OPTS)))
      $(info BRO PLEASE GLES1)
      CURRENT_SOURCES+=hardware/r_gles/r_gles1.c sdl/ogl_es_sdl.c
    else
      $(info BRO PLEASE GLES2)
      CURRENT_SOURCES+=hardware/r_opengl/r_opengl.c sdl/ogl_sdl.c
    endif
  endif
endif

ifndef ANDROID
  opts+=$(CURRENT_OPTS)
  sources+=$(CURRENT_SOURCES)
else
  LOCAL_CFLAGS+=$(CURRENT_OPTS)
  LOCAL_SRC_FILES+=$(CURRENT_SOURCES)
endif
