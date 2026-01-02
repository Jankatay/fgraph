#include "flist.c"


// map a function to functions it uses
struct Map {
  char* fname;       // function's name
  int flen;          // length of fname
  struct Vec fdep;   // array of dependencies
};


// starting from inside a function body, list all the function calls in list of "usermade" ones.
// sets status to 0 on error and 1 on success, nullable.
// -------------------------------------------------------------------------------------------
struct Vec fcalls(FILE* body, struct Vec usermade, int* status) {
  // init 
  struct Vec dependencies = {};
  int statusDump = 0;
  if(!status) status = &statusDump;
  int curlCount = 1;

  // sanitize
  if(!body) return dependencies;
  if(!usermade.cap) {
    *status = 1;
    return dependencies;
  }

  // identifier info
  char fname[BUFSIZE];
  int flen = 0;
  *status = 1;

  // for each token
  enum Token left, right = TOK_ERR;
  while(curlCount) {
    right = ftoken(body, fname, &flen);
    // error doesn't mean the end, continue parsing regardless.
    if(right & (TOK_ERR | TOK_EOF)) *status = 0; 

    // skip comments
    if(tokskip(body, right)) continue;

    // account for curly brackets
    if(right == OP_CURL) curlCount++;
    if(right == CL_CURL) curlCount--;

    // check for function calls
    if((left == TOK_IDENTIFIER) && (right == OP_PAREN)) vset(&dependencies, dependencies.cap, fname, flen);
    right = left;
  }
  
  // body is closed
  return dependencies;
}


// executable form
// -------------------------------------------------------------------------------------------
int main(int argc, char* argv[]) {
  // add each file to a list
  struct Vec vbuf = {};
  for(int i = 1; i < argc; i++) {
    if(!fadd(&vbuf, argv[i])) {
      fprintf(stderr, "File %s not found\n", argv[i]);
      return EXIT_FAILURE;
    }
  }

  // print vbuf
  for(int i = 0; i < vbuf.cap; i++) {
    if(vbuf.arr[i]) printf("%s\n", vbuf.arr[i]);
  }

  vempty(vbuf);
  vfree(vbuf);
  return 0;
}
