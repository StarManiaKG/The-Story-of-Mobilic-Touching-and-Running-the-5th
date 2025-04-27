#
# Makefile for feature flags.
# XTRA Edition!
#

sources+=w_handle.c

NOHW=0
HWRENDER=0
-DHWRENDER=0

ifndef NOHW
	ifeq ($(findstring -DHWRENDER, $(opts)), -DHWRENDER)
		ifeq ($(findstring -DHAVE_GLES2, $(opts)), -DHAVE_GLES2)
			$(info BRO PLEASE)
			sources+=hardware/r_gles/r_gles2.c sdl/ogl_es_sdl.c
		endif
		ifeq ($(findstring -DHAVE_GLES, $(opts)), -DHAVE_GLES)
			$(info BRO PLEASE GLES1)
			sources+=hardware/r_gles/r_gles1.c sdl/ogl_es_sdl.c
		endif
		ifneq ($(findstring -DHAVE_GLES2, $(opts)), -DHAVE_GLES2)
		ifneq ($(findstring -DHAVE_GLES, $(opts)), -DHAVE_GLES)
			$(info BRO PLEASE GLES 2)
			sources+=hardware/r_opengl/r_opengl.c sdl/ogl_sdl.c
		endif
		endif
	endif
endif

sources+=\
	$(call List,android/Sourcefile)\
	$(call List,xtra/Sourcefile)\

opts+=\
	-DTOUCHINPUTS -DNATIVESCREENRES\
	#-DHWRENDER \

# -DHAVE_WHANDLE
# -DHWRENDER -DHAVE_GLES -DHAVE_GLES2 \
