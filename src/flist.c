#ifndef FLIST
#define FLIST
#define _XOPEN_SOURCE 500 

#include "base.c"
#include "vec.c"


// advances a filepos to next function, stopping at beginning of the args.
// puts function name in name and length of len
// returns -1 on error, 0 if none found until EOF, and 1 otherwise.
// -------------------------------------------------------------------------------------------
int ffunc(FILE* filepos, char buf[BUFSIZE], int* len) {
  // init
  if(!filepos) return -1;

  // while getting new tokens
  enum Token right, left = TOK_ERR;
  while((right = ftoken(filepos, buf, len))) {
    // special cases
    if(right == TOK_ERR) return -1;
    if(tokskip(filepos, right)) continue;

    // handle pattern-matches
    if(left == TOK_IDENTIFIER && right == OP_PAREN) return 1;

    // continue looping
    left = right;
  }

  // eof
  return 0;
}


// list all functions inside a fileptr
// returns whatever it could gather so far on error
// -------------------------------------------------------------------------------------------
struct Vec flist(FILE* fileptr) {
  // init and sanitize
  struct Vec result = {};
  size_t vindex = 0;
  if(!fileptr) return result;

  // to be ignored 90% of the time
  char binbuf[BUFSIZE];
  int binlen = 0;
  
  // while going to each function
  int status = 0;
  while((status = ffunc(fileptr, binbuf, &binlen)) > 0) {
    // add to list
    vset(&result, vindex++, binbuf, binlen);

    // skip the function params
    enclose(fileptr, OP_PAREN, CL_PAREN, binbuf);

     // skip the potential function body
    enum Token bintok = ftoken(fileptr, binbuf, &binlen);
    if(bintok == OP_CURL) enclose(fileptr, OP_CURL, CL_CURL, binbuf);
    else if(bintok != TOK_SEMI) return result;
  }

  // eof or error
  return result;
}


// read functions from a file by name and append to optionally empty vector
// returns 0 if could not open file
// -------------------------------------------------------------------------------------------
int ladd(struct Vec* list, char filename[BUFSIZE]) {
  // open the file
  if(!filename || !list) return 0;
  //TODO: fix this also
  FILE* fd = fopen(filename, "r");
  if(!fd) return 0;

  // list all function declarations and append
  // doing it in reverse is reduces syscalls to calloc
  struct Vec vec = flist(fd);
  size_t offset = list->cap;
  for(int i = vec.cap-1; i >= 0; i--) {
    // skip to non-empty
    if(!vec.arr[i]) continue;
    // copy over to destination and free from source
    vset(list, offset+i, vec.arr[i], vec.len[i]);
    free(vec.arr[i]);
  }
  
  // free leftovers and return
  vfree(vec);
  fclose(fd);
  return 1;
}


#endif
