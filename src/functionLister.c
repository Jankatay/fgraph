/* Headerfiles */

#include "base.c"



/* Functions */



// Extract function from source function ptr.
// Will automatically account for newlines too.
// Returns 0 on eof error and 1 on success.
// Name will go in namebuf and namelen. 
// -------------------------------------------------------------------------------------------
int getFunction(FILE* srcfp, char namebuf[BUFSIZE], int* namelen) {
  // init
  char linebuf[BUFSIZE] = "";
  char* lineptr = fgets(linebuf, BUFSIZE, srcfp);
  if(!linebuf) return 0; 

  // expression buffers
  char ebuf[BUFSIZE] = "";
  int elen = 0;

  // while there are tokens on left side.
  enum Token left = getExpression(&lineptr, namebuf, namelen);
  while(left && left != EOF) {
    if(!lineptr) return 0; 

    // extract one to right side.
    enum Token right = getExpression(&lineptr, namebuf, namelen); 

    // Check if right side reached end of line.
    if(right == TOK_EOF) {
      lineptr = fgets(linebuf, BUFSIZE, srcfp);
      continue;
    }

    // On identifier in left followed by parentheses in right.
    if(left == TOK_IDENTIFIER && right == OP_PAREN) {
      // set the file seek.
      fseek(srcfp, -strnlen(lineptr, BUFSIZE), SEEK_CUR);
      // success
      return 1;
    }

    // Otherwise continue parsing with right as new left.
    left = right;
  }

  // no lines left and no function declaration still.
  return 0;
}
// -------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------



// The main function of this headerfile. Lists all files inside "filename" into a buffer "res"
// Returns length of res, negative if terminated due to an error. 
// -------------------------------------------------------------------------------------------
int listFunctions(char filename[BUFSIZE], char res[BUFSIZE]) {
  return 0;
}
// -------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------



// Check tests/lister.c is being listed correctly.
// -------------------------------------------------------------------------------------------
int main() {
  // init
  char buf[BUFSIZE];
  int buflen = 0;

  // get the first function
  FILE* fp = fopen("tests/lister.c", "r");
  while(getFunction(fp, buf, &buflen)) {
    printf("%s\n", buf);
  }

  // print it.
  fclose(fp);
  return 0;
}
// -------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------
