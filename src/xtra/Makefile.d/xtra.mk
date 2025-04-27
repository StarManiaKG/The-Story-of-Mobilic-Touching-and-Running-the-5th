#
# Makefile for feature flags.
# XTRA Edition!
#

sources+=w_handle.c

NOHW=0
HWRENDER=1
-DHWRENDER=1

sources+=\
	$(call List,android/Sourcefile)\
	$(call List,xtra/Sourcefile)\

opts+=-DTOUCHINPUTS -DNATIVESCREENRES # -DHAVE_WHANDLE

#opts+=-DHWRENDER
opts+=-DHWRENDER -DHAVE_GLES -DHAVE_GLES2

CURRENT_OPTS := 

ifndef ANDROID
CURRENT_OPTS=opts
else
CURRENT_OPTS=LOCAL_CFLAGS
endif

ifndef NOHW
$(info CHECKING FOR PROPER OPENGL...)
ifeq ($(findstring -DHWRENDER, $(CURRENT_OPTS)), -DHWRENDER)
ifeq ($(findstring -DHAVE_GLES2, $(CURRENT_OPTS)), -DHAVE_GLES2)
$(info BRO PLEASE)
sources+=hardware/r_gles/r_gles2.c sdl/ogl_es_sdl.c
endif
ifeq ($(findstring -DHAVE_GLES, $(CURRENT_OPTS)), -DHAVE_GLES)
$(info BRO PLEASE GLES1)
sources+=hardware/r_gles/r_gles1.c sdl/ogl_es_sdl.c
endif
ifneq ($(findstring -DHAVE_GLES2, $(CURRENT_OPTS)), -DHAVE_GLES2)
ifneq ($(findstring -DHAVE_GLES, $(CURRENT_OPTS)), -DHAVE_GLES)
$(info BRO PLEASE GLES 2)
sources+=hardware/r_opengl/r_opengl.c sdl/ogl_sdl.c
endif
endif
endif
endif
