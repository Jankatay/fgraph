/* Headerfiles */


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>



/* Definitions */


// Default buffer sizes
// -------------------------------------------------------------------------------------------
#define BUFSIZE 255 //  lines and filenames.
// -------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------



// General tokenization to make stuff simpler.
// -------------------------------------------------------------------------------------------
enum Token {
  TOK_EOF = 0,        // EOF or end of line
  TOK_IDENTIFIER = 1, // C identifiers
  TOK_ERR = 2,        // error
  OP_CURL = 4,        // '{'
  CL_CURL = 8,        // '}'
  OP_PAREN = 16,      // '('
  CL_PAREN = 32,      // ')'
  TOK_COMM = 64,      // ','
  other = -1,         // '*'
};
// -------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------



/* Functions */


// Follow a char up to the end and ensure it's an identifier.
// Store identifier in identstr and length in identlen
// returns 0 on error and 1 on success
// -------------------------------------------------------------------------------------------
int getIdentifier(char** str, char identres[BUFSIZE], int* identlen) {
  // sanitize
  if(!identlen || !str || !(*str)) return 0;
  if(!isalpha(**str)) return 0;
  
  // init
  *identlen = 0;
  char* c = NULL; 

  // until the delimiter
  for(c = *str; c && *c != EOF; *c++) {
    if(*identlen == (BUFSIZ-1)) return 0;
    // check delimiter
    if(isspace(*c)) break;
    if(ispunct(*c)) break;
    // check error
    if(!isalnum) return 0;
    // add to list otherwise.
    identres[ (*identlen)++ ] = *c;
  }
 
  // success
  *str = c;
  identres[ (*identlen)++ ] = '\0';
  return 1; 
}
// -------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------



// Get an individual expression from the "currentLine." 
// The expression can be an identifier or something like '('
// In case of identifiers, writes the expression to resexp and sets lenexp to the length.
// Skips everything else while parsing
// Returns the token
// -------------------------------------------------------------------------------------------
enum Token getExpression(char** currentLine, char resexp[BUFSIZE], int* lenexp) {
  if(!currentLine || !(*currentLine)) return TOK_ERR;

  // For each character.
  for(char* c = *currentLine; *c && *c != EOF; c++) {

    // Check trivial symbols
    char* backup = *currentLine;
    *currentLine = c+1;
    if(*c == '{') { return OP_CURL; }
    if(*c == '}') { return CL_CURL; }
    if(*c == '(') { return OP_PAREN; }
    if(*c == ')') { return CL_PAREN; }
    if(*c == ',') { return TOK_COMM; }
    *currentLine = backup;

    // Follow up with the identifiers.
    if(isalpha(*c)) {
      int success = getIdentifier(&c, resexp, lenexp);
      *currentLine = c;
      return success? TOK_IDENTIFIER : TOK_ERR;
    }
  }

  // end of file.
  return TOK_EOF;
}
// -------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------



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
  while(left) {
    if(!linebuf) return 0; 

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
      fseek(srcfp, *namelen - strnlen(linebuf, BUFSIZE), SEEK_CUR);
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
