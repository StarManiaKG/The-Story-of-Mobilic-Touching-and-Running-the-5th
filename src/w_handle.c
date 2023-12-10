// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// Copyright (C) 2020 by Sonic Team Junior.
// Copyright (C) 2020-2023 by SRB2 Mobile Project.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  w_handle.c
/// \brief WAD file handle

#include "w_handle.h"
#include "w_wad.h"
#include "m_misc.h"

#include "i_system.h"
#include "console.h"

size_t File_StandardSizeImpl(FILE *f)
{
	size_t cur, length;

	cur = ftell(f);
	fseek(f, 0, SEEK_END);
	length = ftell(f);
	fseek(f, cur, SEEK_SET);

	return length;
}

#ifndef HAVE_WHANDLE

void *File_Open(const char *filename, const char *filemode, fhandletype_t type)
{
	(void)type;
	return fopen(filename, filemode);
}

int File_Close(void *stream)
{
	return fclose(stream);
}

int File_CheckError(void *stream)
{
	//return ferror(stream);
}

#else

//
// Standard library file operations
//

// File read / seek / tell
size_t      File_StandardRead      (void *f, void *ptr, size_t size, size_t count);
int         File_StandardSeek      (void *stream, long int offset, int origin);
long int    File_StandardTell      (void *stream);
size_t      File_StandardSize      (void *stream);
int         File_StandardGetChar   (void *f);
char       *File_StandardGetString (void *f, char *str, int num);

// File close / error
int         File_StandardClose     (void *f);
const char *File_StandardError     (void *f);
int         File_StandardEOF       (void *f);

//
// SDL_RWops file operations
//

#ifdef HAVE_SDL
// File read / seek / tell
size_t      File_SDLRead      (void *f, void *ptr, size_t size, size_t count);
int         File_SDLSeek      (void *stream, long int offset, int origin);
long int    File_SDLTell      (void *stream);
size_t      File_SDLSize      (void *f);

int         File_SDLGetChar   (void *f);
char       *File_SDLGetString (void *f, char *str, int num);

// File close / error
int         File_SDLClose     (void *f);
const char *File_SDLError     (void *f);
int         File_SDLEOF       (void *f);
#endif

// Open a file handle.
void *File_Open(const char *filename, const char *filemode, fhandletype_t type)
{
	filehandle_t *handle = calloc(sizeof(filehandle_t), 1);

	handle->type = type;

	if (type == FILEHANDLE_STANDARD)
	{
		handle->read = &File_StandardRead;
		handle->seek = &File_StandardSeek;
		handle->tell = &File_StandardTell;
		handle->size = &File_StandardSize;
		handle->getchar = &File_StandardGetChar;
		handle->getstring = &File_StandardGetString;
		handle->close = &File_StandardClose;
		handle->error = &File_StandardError;
		handle->eof = &File_StandardEOF;
		handle->file = fopen(filename, filemode);
	}
#ifdef HAVE_SDL
	else if (type == FILEHANDLE_SDL)
	{
		handle->read = &File_SDLRead;
		handle->seek = &File_SDLSeek;
		handle->tell = &File_SDLTell;
		handle->size = &File_SDLSize;
		handle->getchar = &File_SDLGetChar;
		handle->getstring = &File_SDLGetString;
		handle->close = &File_SDLClose;
		handle->error = &File_SDLError;
		handle->eof = &File_SDLEOF;
		handle->file = SDL_RWFromFile(filename, filemode);
	}
#endif
	else
		I_Error("File_Open: unknown file handle type!");

	// File not found
	if (!handle->file)
	{
		free(handle);
		return NULL;
	}

	return handle;
}

// Close the file handle.
int File_Close(void *f)
{
	filehandle_t *handle = (filehandle_t *)f;
	int ok = handle->close(handle->file);
	if (handle->lasterror)
		free(handle->lasterror);
	free(handle);
	return ok;
}

// Check for file read errors.
int File_CheckError(void *f)
{
	filehandle_t *handle = (filehandle_t *)f;

	switch (handle->type)
	{
		case FILEHANDLE_STANDARD:
			return ferror((FILE *)handle->file);
#ifdef HAVE_SDL
		case FILEHANDLE_SDL:
			return 0;
#endif
		default:
			break;
	}

	return 0;
}

//
// Standard library file operations
//

// Read bytes from the stream into a buffer.
size_t File_StandardRead(void *f, void *ptr, size_t size, size_t count)
{
	filehandle_t *handle = (filehandle_t *)f;
	return fread(ptr, size, count, (FILE *)handle->file);
}

// Seek to the specified position in the stream.
int File_StandardSeek(void *f, long int offset, int origin)
{
	filehandle_t *handle = (filehandle_t *)f;
	return fseek((FILE *)handle->file, offset, origin);
}

// Get the current position in the stream.
long int File_StandardTell(void *f)
{
	filehandle_t *handle = (filehandle_t *)f;
	return ftell((FILE *)handle->file);
}

// Get the total size of the stream.
size_t File_StandardSize(void *f)
{
	filehandle_t *handle = (filehandle_t *)f;
	return (size_t)File_StandardSizeImpl((FILE *)handle->file);
}

// Read a single character from the file stream.
int File_StandardGetChar(void *f)
{
	filehandle_t *handle = (filehandle_t *)f;
	return fgetc((FILE *)handle->file);
}

char *File_StandardGetString(void *f, char *str, int num)
{
	filehandle_t *handle = (filehandle_t *)f;
	return fgets(str, num, (FILE *)handle->file);
}

// Close the file handle.
int File_StandardClose(void *f)
{
	return fclose((FILE *)f);
}

// Get latest file error.
const char *File_StandardError(void *f)
{
	return M_FileError((FILE *)(((filehandle_t *)f)->file));
}

// Check for end-of-file.
int File_StandardEOF(void *f)
{
	return feof((FILE *)(((filehandle_t *)f)->file));
}

//
// SDL_RWops file operations
//

#ifdef HAVE_SDL
static void File_SDLSetError(filehandle_t *handle)
{
	const char *errorstring = SDL_GetError();
	if (handle->lasterror)
		free(handle->lasterror);
	handle->lasterror = calloc(strlen(errorstring) + 1, 1);
	strcpy(handle->lasterror, errorstring);
}

// Read bytes from the stream into a buffer.
size_t File_SDLRead(void *f, void *ptr, size_t size, size_t count)
{
	filehandle_t *handle = (filehandle_t *)f;
	size_t read = SDL_RWread((struct SDL_RWops *)handle->file, ptr, size, count);
	File_SDLSetError(handle);
	return read;
}

// Seek to the specified position in the stream.
int File_SDLSeek(void *f, long int offset, int origin)
{
	filehandle_t *handle = (filehandle_t *)f;
	int position, type = RW_SEEK_SET;

	// set seek type
	if (origin == SEEK_CUR)
		type = RW_SEEK_CUR;
	else if (origin == SEEK_END)
		type = RW_SEEK_END;

	position = (int)SDL_RWseek((struct SDL_RWops *)handle->file, offset, type);
	File_SDLSetError(handle);
	return (position == -1) ? -1 : 0;
}

// Get the current position in the stream.
long int File_SDLTell(void *f)
{
	filehandle_t *handle = (filehandle_t *)f;
	long int position = (long int)SDL_RWtell((struct SDL_RWops *)handle->file);
	File_SDLSetError(handle);
	return position;
}

// Get the total size of the stream.
size_t File_SDLSize(void *f)
{
	filehandle_t *handle = (filehandle_t *)f;
	size_t sz = (size_t)SDL_RWsize((struct SDL_RWops *)handle->file);
	File_SDLSetError(handle);
	return sz;
}

// Read a single character from the file stream.
int File_SDLGetChar(void *f)
{
	filehandle_t *handle = (filehandle_t *)f;
	char c;
	if (!SDL_RWread((struct SDL_RWops *)handle->file, &c, sizeof(char), 1))
	{
		File_SDLSetError(handle);
		return EOF;
	}
	return (int)c;
}

char *File_SDLGetString(void *f, char *str, int num)
{
	filehandle_t *handle = (filehandle_t *)f;
	if (!SDL_RWread((struct SDL_RWops *)handle->file, str, sizeof(char), num - 1))
	{
		File_SDLSetError(handle);
		return NULL;
	}
	str[num-1] = '\0';
	return str;
}

// Close the file handle.
int File_SDLClose(void *f)
{
	return SDL_RWclose((struct SDL_RWops *)f);
}

// Get latest file error.
const char *File_SDLError(void *f)
{
	return (const char *)(((filehandle_t *)f)->lasterror);
}

// Check for end-of-file.
int File_SDLEOF(void *f)
{
	filehandle_t *handle = (filehandle_t *)f;
	long int filesize, position;

	filesize = (long int)SDL_RWsize((struct SDL_RWops *)handle->file);
	if (filesize < 0)
	{
		File_SDLSetError(handle);
		return 1;
	}

	position = (long int)SDL_RWtell((struct SDL_RWops *)handle->file);
	if (position == -1)
		return 1;

	if (position >= filesize)
		return 1;

	return 0;
}

#endif // HAVE_SDL

#endif
