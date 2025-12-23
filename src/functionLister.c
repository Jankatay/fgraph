#include "base.c"


// Skip up to a bitwise-or of optional token targets.
// Most recent identifier info will go in namebuf and namelen. 
// Will automatically account for newlines too.
// Will return current and previous token on success or NULL on error.
// -------------------------------------------------------------------------------------------
struct Tokens* skip(FILE* srcfp, enum Token targets, char namebuf[BUFSIZE], int* namelen) {
  if(!srcfp || !namelen) return NULL;
  // init
  char linebuf[BUFSIZE] = "";
  char* lineptr = fgets(linebuf, BUFSIZE, srcfp);
  if(!linebuf) return 0; 
  char ebuf[BUFSIZE] = "";
  int elen = 0;

  // while there are tokens on left side.
  enum Token left = getExpression(&lineptr, namebuf, namelen);
  while(left && left != EOF) {
    if(!lineptr) return NULL;

    // extract one to right side.
    enum Token right = getExpression(&lineptr, namebuf, namelen); 

    // Check if right side reached end of line.
    if(right == TOK_EOF) {
      lineptr = fgets(linebuf, BUFSIZE, srcfp);
      continue;
    }

    // Upon reaching the target
    if(right & targets) {
      // success, rewind buffer and return info.
      fseek(srcfp, -strnlen(lineptr, BUFSIZE), SEEK_CUR);
      struct Tokens* resTokens = (struct Tokens*)calloc(1, sizeof(struct Tokens));
      *resTokens = (struct Tokens){left, right};
      return resTokens;
    }

    // Otherwise continue parsing with right as new left.
    left = right;
  }

  // no lines left and no matches still
  return NULL;
}


// The main function of this headerfile. Lists all files inside "filename" into a buffer "res"
// Returns length of res, negative if terminated due to an error. 
// -------------------------------------------------------------------------------------------
int listFunctions(char filename[BUFSIZE], char res[BUFSIZE]) {
  return 0;
}


// Check tests/lister.c is being listed correctly.
// -------------------------------------------------------------------------------------------
int main() {
  // init
  char buf[BUFSIZE];
  int buflen = 0;

  // get the first function
  FILE* fp = fopen("tests/lister.c", "r");
  struct Tokens* pattern = NULL;
  while( (pattern = skip(fp, OP_PAREN, buf, &buflen)) ) {
    if(pattern->left == TOK_IDENTIFIER && pattern->right == OP_PAREN) printf("%s\n", buf);
    skip(fp, CL_PAREN, buf, &buflen);
  }

  // print it.
  fclose(fp);
  return 0;
}
