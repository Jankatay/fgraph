#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ftw.h>
#include <string.h>
#include "vec.c"


// important global variables 
// -------------------------------------------------------------------------------------------
struct Vec finfo = {};           // Will be appended result of "files" function
char* fext = NULL;               // Optional extensions for the "files" function
struct Vec fexc = {};            // Exclude these files


// function for the ntfw(3), appends source code files under a directory
// appends all files found to global variable "finfo"
// can setup an optionally space-seperated list of extensions 
// for example fext = ".c .cpp .h" will only append files that have those extensions
// -------------------------------------------------------------------------------------------
static int files(const char* topdir, const struct stat* sb, int tflag, struct FTW* ftwbuf) {
  // skip [REDACTED]
  if(!topdir) return 0; 

  // skip hidden files like ".git" by default.
  // char* filename = strrchr(topdir, '/');
  // if(filename && *filename[1] == '.') return 0;

  // skip excluded files.
  if(vfind(fexc, topdir) >= 0) return 0;

  // append
  if(tflag == FTW_F) {
    vset(&finfo, finfo.cap, topdir, strnlen(topdir, BUFSIZE));
  }

  // continue
  return 0; 
}
