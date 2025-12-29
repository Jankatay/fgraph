#include "base.c"
#include "vector.c"


// get the next function from a fileptr
// returns whatever it could gather so far on error
// -------------------------------------------------------------------------------------------
struct Vec listFunctions(FILE* fileptr, char returnBuffer[BUFSIZE]) {
  struct Vec result = vnew(0);
  if(!fileptr) return result;
  size_t resultIndex = 0;
  int returnLength = 0;

  // to be ignored when used as buffer
  char binbuf[BUFSIZE];
  int binlen = 0;
  
  // get next token
  enum Token right, left = token(fileptr, returnBuffer, &returnLength);
  if(!left) return result;

  // while getting new tokens
  while((right = token(fileptr, returnBuffer, &returnLength))) {
    if(right == TOK_ERR) return result;

    // handle pattern-matches
    if(left == TOK_IDENTIFIER && right == OP_PAREN) {
      // add to list
      vset(&result, resultIndex++, returnBuffer, returnLength);

      // skip the function params
      enclose(fileptr, OP_PAREN, CL_PAREN, binbuf);

       // skip the potential function body
      enum Token bintok = token(fileptr, binbuf, &binlen);
      if(bintok == OP_CURL) enclose(fileptr, OP_CURL, CL_CURL, binbuf);
      else if(bintok != TOK_SEMI) return result;
    }

    // continue looping
    left = right;
  }
  // eof
  return result;
}


// Check tests/lister.c is being listed correctly.
// -------------------------------------------------------------------------------------------
int main() {
  // get the first function
  FILE* fp = fopen("tests/lister.c", "r");
  char buf[BUFSIZE] = "";
  int buflen = 0;

  struct Vec v = listFunctions(fp, buf);
  for(int i = 0; i < v.cap; i++) {
    if(v.arr[i]) printf("%s\n", v.arr[i]);
  }

  // success
  fclose(fp);
  return 0;
}
