#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include "vec.c"


// Resolve files under a path.
// Ensure "codefile" is a readable file and append it to the list "dest"
// If it's a directory, will go through all files recursively and append them instead
// If it's a directory and ext isn't NULL, adds only files that have the correct extensions. 
// chext tells it if you want to check for extensions or not, will automatically turn on when dir.
// Extensions is a string ideally seperated by spaces, like ".c .cpp .h.hpp" -> "*.c", "*.cpp", ".h", ".hpp"
// Returns 0 if not a readable file or directory or memory error.
// -------------------------------------------------------------------------------------------
int resolvePaths(char path[BUFSIZE], struct Vec* dest, char ext[], int chext) {
  if(!path) return 0;

  // get file info
  struct stat sb = {};
  if(stat(path, &sb) < 0) return 0;

  // Base case, a file. Append and return
  if(sb.st_mode & S_IFMT & S_IFREG) {
    char* extension = strrchr(path, '.');
    // run an extension check
    if(chext && !strstr(ext, extension)) return 0; 
    // append on success
    return vset(dest, dest->cap, path, strnlen(path, BUFSIZE));
  }

  // it MUST be a directory if not file, alternatively error.
  int isdir = sb.st_mode & S_IFMT & S_IFDIR;
  if(!isdir) return 0;

  // TODO: Get all the files under a directory and recurse
  return 1;
}
