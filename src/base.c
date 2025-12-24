#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

// Default buffer size for line and filenames
#define BUFSIZE 255 

// General tokenization to make stuff simpler.
enum Token {
  TOK_EOF = 0,        // EOF or end of line
  TOK_IDENTIFIER = 1, // C identifiers
  TOK_ERR = 2,        // error
  OP_CURL = 4,        // '{'
  CL_CURL = 8,        // '}'
  OP_PAREN = 16,      // '('
  CL_PAREN = 32,      // ')'
  TOK_COMM = 64,      // ','
  TOK_SEMI = 128,     // ';'
  other = -1,         // '*'
};

// pair of tokens in order.
struct Tokens {
  enum Token left;
  enum Token right;
};


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
    if(*c == '{') return OP_CURL;
    if(*c == '}') return CL_CURL;
    if(*c == '(') return OP_PAREN;
    if(*c == ')') return CL_PAREN;
    if(*c == ',') return TOK_COMM;
    if(*c == ';') return TOK_SEMI;
    *currentLine = backup;

    // Follow up with the identifiers.
    if(isalpha(*c)) {
      int success = getIdentifier(&c, resexp, lenexp);
      *currentLine = c;
      return success? TOK_IDENTIFIER : TOK_ERR;
    }

    // skip everything else
  }

  // end of file.
  return TOK_EOF;
}
