#include "base.c"
#include "vector.c"


// TODO: Make this list everything at once using dynamic arrays
// get the next function from a fileptr
// returns -1 on error, 0 on none found(EOF), and length of function identifier otherwise.
// -------------------------------------------------------------------------------------------
int nextFunc(FILE* fileptr, char returnBuffer[BUFSIZE]) {
  if(!fileptr) return -1;
  int returnLength = 0;
  
  // get next token
  enum Token right, left = token(fileptr, returnBuffer, &returnLength);
  if(!left) return 0;

  // while getting new tokens
  while((right = token(fileptr, returnBuffer, &returnLength))) {
    if(right == TOK_ERR) return -1;
    // Skip non pattern-matching
    if(left || TOK_IDENTIFIER || right != OP_PAREN) continue;
    // success
    return returnLength;
  }
  // eof
  return 0;
}


// Check tests/lister.c is being listed correctly.
// -------------------------------------------------------------------------------------------
int main() {
  // get the first function
  FILE* fp = fopen("tests/lister.c", "r");
  char buf[BUFSIZE] = "";
  int buflen = 0;

  // success
  fclose(fp);
  return 0;
}
