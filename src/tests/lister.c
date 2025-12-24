#include "base.c"


// Skip up to a bitwise-or of optional token targets.
// Most recent identifier info will go in namebuf and namelen. 
// Will automatically account for newlines too.
// Will return current and previous token on success or NULL on error.
// -------------------------------------------------------------------------------------------
struct Tokens skip(FILE* srcfp, enum Token targets, char namebuf[BUFSIZE], int* namelen) {
  if(!srcfp || !namelen) return (struct Tokens){};
  // init
  char linebuf[BUFSIZE] = "";
  char* lineptr = fgets(linebuf, BUFSIZE, srcfp);
  if(!linebuf) return (struct Tokens){}; 
  char ebuf[BUFSIZE] = "";
  int elen = 0;

  // while there are tokens on left side.
  enum Token left = getExpression(&lineptr, namebuf, namelen);
  while(left != TOK_ERR) {
    if(!lineptr) return (struct Tokens){left, TOK_EOF};

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
      return (struct Tokens){left, right};
    }

    // Otherwise continue parsing with right as new left.
    left = right;
  }

  // no lines left and no matches still
  return (struct Tokens){};
}


// Follow a function's params to its closing in file pointer "paramsFileptr."
// Starting after the OP_PAREN for params
// Can be of form 'foo();' or 'foo(){...'
// Returns 0 on error and 1 otherwise
//--------------------------------------------------------------------------------------------
int encloseParams(FILE* paramsFileptr) {
  // increment on '(' and decrement on ')' and succeed once 0
  int parens = 1; 

  // somewhat unnecesary variables
  struct Tokens pattern = {};
  char dump[BUFSIZE] = "";
  int dumplen = 0;
  
  // go up to closest [(){;]
  enum Token importantTokens = OP_PAREN | CL_PAREN | OP_CURL | TOK_SEMI;
  char firstRun = 1;
  while("Getting valid patterns"){
    pattern = skip(paramsFileptr, TOK_ERR | importantTokens, dump, &dumplen);
    int errorPattern = (pattern.left == TOK_ERR) && (pattern.right == TOK_ERR);
    if(errorPattern) break;
    if(!pattern.left && !pattern.right) return 1; // eof

    // disect the pattern
    enum Token prevToken = pattern.left;
    enum Token mainToken = pattern.right;
    
    // exit on error
    if(mainToken == TOK_ERR) return 0;

    // assume parentheses
    int parenChanged = 1; 

    // check prevToken on first iteration
    if(firstRun && prevToken == CL_PAREN) parens--;
    firstRun = 0;

    // manage parentheses
    if(mainToken == OP_PAREN) parens++; 
    else if(mainToken == CL_PAREN) parens--; 
    else parenChanged = 0;

    // continue looping if assumption was correct or parentheses still open
    if(parenChanged || parens) continue;

    // check for success
    if(prevToken != CL_PAREN) continue;
    if(OP_CURL | TOK_SEMI & mainToken) return 1; 
  }

  // fail
  return 0;
}


// The main function of this headerfile. Lists all files inside "code file" into a buffer "res"
// Returns length of res, negative if terminated due to an error. 
// -------------------------------------------------------------------------------------------
int listFunctions(FILE* codeFile, char res[BUFSIZE]) {
  // init
  struct Tokens pattern = {};
  char dump[BUFSIZE], buf[BUFSIZE] = "";
  int dumplen, buflen = 0;

  // go up to nearest '('
  while("Getting the pattern") {
    pattern = skip(codeFile, OP_PAREN, buf, &buflen);
    int errorStatus = (pattern.left == TOK_ERR) && (pattern.right == TOK_ERR);
    if(errorStatus) break;
    if(!pattern.left && !pattern.right) break; // eof
    
    // extract necesary info and free pattern.
    char isFunctionDeclaration = (pattern.left == TOK_IDENTIFIER) && (pattern.right == OP_PAREN);

    // Try again if not part of a function declaration
    if(!isFunctionDeclaration) continue;
    if(!encloseParams(codeFile)) continue;

    // Print the function identifier.
    printf("%s\n", buf);
  }
  return 0;
}


// Check tests/lister.c is being listed correctly.
// -------------------------------------------------------------------------------------------
int main() {
  // get the first function
  char buf[BUFSIZE] = "";
  FILE* fp = fopen("tests/lister.c", "r");
  listFunctions(fp, buf);

  // success
  fclose(fp);
  return 0;
}
