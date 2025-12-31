#ifndef FLIST
#define FLIST

#include "base.c"
#include "vec.c"


// get the next function from a fileptr
// returns whatever it could gather so far on error
// -------------------------------------------------------------------------------------------
struct Vec flist(FILE* fileptr) {
  // init
  struct Vec result = {};
  if(!fileptr) return result;
  size_t vindex = 0;
  int returnLength = 0;

  // a trashbin to be ignored 90% of the time
  char binbuf[BUFSIZE];
  int binlen = 0;
  
  // get next token
  enum Token right, left = TOK_ERR;

  // while getting new tokens
  while((right = ftoken(fileptr, binbuf, &binlen))) {
    if(right == TOK_ERR) return result;
    if(right == TOK_SKIP) {
      left = skip(fileptr, TOK_EOL, binbuf, &binlen);
      continue;
    }

    // handle pattern-matches
    if(left == TOK_IDENTIFIER && right == OP_PAREN) {
      // add to list
      vset(&result, vindex++, binbuf, binlen);

      // skip the function params
      enclose(fileptr, OP_PAREN, CL_PAREN, binbuf);

       // skip the potential function body
      enum Token bintok = ftoken(fileptr, binbuf, &binlen);
      if(bintok == OP_CURL) enclose(fileptr, OP_CURL, CL_CURL, binbuf);
      else if(bintok != TOK_SEMI) return result;
    }

    // continue looping
    left = right;
  }
  // eof
  return result;
}


// read functions from a file by name and append to optionally empty vector
// returns 0 if could not open file
// -------------------------------------------------------------------------------------------
int fadd(struct Vec* list, char filename[BUFSIZE]) {
  // open the file
  if(!filename || !list) return 0;
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
