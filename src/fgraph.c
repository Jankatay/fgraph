#include "flist.c"


// Graph to map a function to its dependencies.
// -------------------------------------------------------------------------------------------
struct Graph {
  struct Vec names;   // each function name in order
  struct Vec* map;    // mapping of names to dependencies
  int len;            // amt of vertices
};


// follow a function from right after start of the curly brackets until the end
// returns a vector of all identifiers that were in the vector "usermade"
struct Vec fdepends(FILE* fbody, struct Vec usermade) {
  // init and sanitize
  struct Vec res = {};
  int resi = 0;
  if(!fbody) return res;
  if(!usermade.cap) return res;

  // buffers
  char idbuf[BUFSIZE];
  int idlen = 0;

  // while curly braces are open
  int curly = 1;
  while(curly) { 
    // get the next token and skip the comments while at that.
    enum Token next = ftoken(fbody, idbuf, &idlen);
    if(tokskip(fbody, next)) continue;

    // failure
    if(!next || (next == TOK_ERR)) return res;

    // handle curly braces
    if(next == OP_CURL) curly++;
    if(next == CL_CURL) curly--;

    // add to result if identifier is usermade
    if(next != TOK_IDENTIFIER) continue;
    if(vfind(usermade, idbuf) >= 0) {
      vset(&res, resi++, idbuf, idlen);
    }
  }

  // success
  return res;
}


// starting from inside a function body, list all the function calls in list of "usermade" ones.
// sets status to 0 on error and 1 on success, nullable.
// -------------------------------------------------------------------------------------------
struct Graph fcalls(FILE* body, struct Vec usermade, int* status) {
  // sanitize
  struct Graph res = {};
  if(!body) return res;
  if(!usermade.cap) return res;

  // init status
  int sdump = 0;
  if(!status) status = &sdump;
  if(*status) *status = 0;

  // init graph
  int gindex = 0;
  res = (struct Graph){
    .names = {},
    .map = calloc(usermade.cap, sizeof(struct Vec)),
    .len = usermade.cap
  };

  // to store function identifiers
  char fid[BUFSIZE];
  int fidlen = 0;
  
  // while going to each function
  while(ffunc(body, fid, &fidlen) > 0) {
    // duplicate the id into graph.names[] 
    vset(&res.names, gindex, fid, fidlen);

    // skip the function params
    enclose(body, OP_PAREN, CL_PAREN, IGNORE);

     // skip if there is a semicolon
    enum Token next = ftoken(body, fid, &fidlen);
    if(next == TOK_SEMI) continue;

    // return what could be gathered so far on error.
    if(next != OP_CURL) return res; 

    // add in the dependencies and make index no longer overwriteable.
    //enclose(fileptr, OP_CURL, CL_CURL, IGNORE);
    res.map[gindex++] = fdepends(body, usermade);
  }

  // eof or error
  return res;
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

  // get map of dependencies
  FILE* target = fopen(argv[1], "r");
  struct Graph res = fcalls(target, vbuf, NULL);

  for(int i = 0; i < res.names.cap; i++) {
    for(int j = 0; j < res.map[i].cap; j++) {
      printf("%s -> %s\n", res.names.arr[i], res.map[i].arr[j]);
    }
  }

  vempty(vbuf);
  vfree(vbuf);
  return 0;
}
