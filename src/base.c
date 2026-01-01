#ifndef BASE
#define BASE



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
  TOK_IDENTIFIER = 1, // C identifiers plus '//', '/*', and '*/'
  TOK_ERR = 2,        // error
  OP_CURL = 4,        // '{'
  CL_CURL = 8,        // '}'
  OP_PAREN = 16,      // '('
  CL_PAREN = 32,      // ')'
  TOK_COMMA = 64,     // ','
  TOK_SEMI = 128,     // ';'
  TOK_EOL = 256,      // '\n'
  TOK_SKIP = 512,     // line-skippers, '//' and '#'
  OP_COMM = 1024,     // '/*'
  CL_COMM = 2048,     // '*/'
  other = -1,         // * 
};


// Follow a char up to the end and ensure it's an identifier.
// Store identifier in identstr and length in identlen
// returns 0 on error and 1 on success
// -------------------------------------------------------------------------------------------
int identifier(char** str, char identres[BUFSIZE], int* identlen) {
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


// Get an individual token from the "currentLine." 
// In case of identifiers, writes the expression to resexp and sets lenexp to the length.
// Skips everything else while parsing
// Returns the token
// -------------------------------------------------------------------------------------------
enum Token token(char** currentLine, char resexp[BUFSIZE], int* lenexp) {
  if(!currentLine || !(*currentLine)) return TOK_ERR;

  // For each character.
  for(char* c = *currentLine; *c && *c != EOF; c++) {

    // Check trivial symbols
    *currentLine = c+1;
    if(*c == '{') return OP_CURL;
    if(*c == '}') return CL_CURL;
    if(*c == '(') return OP_PAREN;
    if(*c == ')') return CL_PAREN;
    if(*c == ',') return TOK_COMMA;
    if(*c == ';') return TOK_SEMI;
    if(*c == '#') return TOK_SKIP;
    if(*c == '\n') return TOK_EOL;
    *currentLine = c-1;

    // non-trivial symbols
    *currentLine = c+2;
    if(*c == '/' && *(c+1) == '/') return TOK_SKIP;
    if(*c == '/' && *(c+1) == '*') return OP_COMM;
    if(*c == '*' && *(c+1) == '/') return CL_COMM;
    *currentLine = c+2;

    // Follow up with the identifiers.
    if(isalpha(*c)) {
      int success = identifier(&c, resexp, lenexp);
      *currentLine = c;
      return success? TOK_IDENTIFIER : TOK_ERR;
    }

    // skip everything else
  }

  // end of file.
  return TOK_EOF;
}


// Get next token from a file.
// If an identifier, string will be stored in namebuf and namelen. 
// Will automatically skip whitespace including newlines.
// -------------------------------------------------------------------------------------------
enum Token ftoken(FILE* srcfp, char namebuf[BUFSIZE], int* namelen) {
  if(!srcfp || !namelen) return TOK_ERR;
  // init
  char linebuf[BUFSIZE] = "";
  char* lineptr = fgets(linebuf, BUFSIZE, srcfp);
  if(!linebuf) return TOK_EOF; 

  // loop
  char ebuf[BUFSIZE] = "";
  int elen = 0;
  while("there are tokens") {
    enum Token nextToken = token(&lineptr, namebuf, namelen);
    if(!lineptr) return TOK_EOF; // 

    // Take next line if reached end of line
    if(nextToken == TOK_EOF) {
      lineptr = fgets(linebuf, BUFSIZE, srcfp);
      continue;
    }

    // success, rewind buffer and return info.
    fseek(srcfp, -strnlen(lineptr, BUFSIZE), SEEK_CUR);
    return nextToken;
  }
}


// Skip a file to a bitwise-or of tokens, will return which token made it stop.
// Will hold the last identifier encountered, returning token inclusive.
// Will return TOK_EOF if and only if file ends. 
// TOK_ERR and TOK_EOF are hardcoded. You do not need to add them to stopsign.
// -------------------------------------------------------------------------------------------
enum Token skip(FILE* search, enum Token stopsign, char last[BUFSIZE], int* lastsize) {
  stopsign |= TOK_ERR;
  // bad input
  if(!search) return TOK_ERR;

  // just in case humans are stupid
  if(!stopsign) {
    fseek(search, 0, SEEK_END);
    return TOK_EOF;
  }

  // while getting tokens
  enum Token head = TOK_ERR;
  while((head = ftoken(search, last, lastsize))) {
    if(head & stopsign) return head;
  }

  // file ended
  return TOK_EOF;
}


// handle skipping/comments in a file based on last token
// returns 1 if skipped anything, and 0 otherwise
// ----------------------------------------------------------------
int tokskip(FILE* filepos, enum Token last) {
  if(!filepos) return 0;

  // init
  char ignore[BUFSIZE];
  int ignorelen = 0;
  
  // handle '//' and '#'
  if(last == TOK_SKIP) {                                      
    skip(filepos, TOK_EOL, ignore, &ignorelen);     
    return 1;
  }                                                             

  // handle '/*'
  if(last == OP_COMM) {                                        
    skip(filepos, CL_COMM, ignore, &ignorelen);             
    return 1;
  }                                                             

  // didn't skip anything
  return 0;
}


// follow through enclosing using openings and closings
// Example : opening='(', closing=')' with fseek='_' of "(_((a)b),())hello" would skip to "hello"
// will put last identifier in trashbin and return length or -1 on error
// it's named trashbin because 9 times out of 10 in practice you will want to ignore it anyways.
// -------------------------------------------------------------------------------------------
int enclose(FILE* afterParen, enum Token opening, enum Token closing, char trashbin[BUFSIZE]) {
  // sanitize
  if(!afterParen) return -1;

  // the rest of the program is just getting ended down to 0
  int ended = 1;

  // for each token
  int reslen = -1;
  enum Token tok = 0;
  while((tok = ftoken(afterParen, trashbin, &reslen))) {
    // skip comments
    if(tokskip(afterParen, tok)) continue;
    // move the goalpoast as necessary
    if(tok & opening) ended++;
    if(tok & closing) ended--;
    // stop at the end
    if(!ended) return reslen;
  }
  
  // fail
  return -1;
}


#endif
