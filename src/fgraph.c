#include "flist.c"


// Graph to map a function to its dependencies.
// -------------------------------------------------------------------------------------------
struct Graph {
  struct Vec names;   // each function name in order
  struct Vec* map;    // mapping of names to dependencies
  int len;            // amt of vertices
};


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
    res.names.arr[gindex++] = strndup(fid, fidlen);

    // skip the function params
    enclose(body, OP_PAREN, CL_PAREN, IGNORE);

     // skip if there is a semicolon
    enum Token next = ftoken(body, fid, &fidlen);
    if(next == TOK_SEMI) continue;

    // return what could be gathered so far on error.
    if(next != OP_CURL) return res; 
    
    // TODO: handle function bodies
    // enclose(fileptr, OP_CURL, CL_CURL, IGNORE);
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


  vempty(vbuf);
  vfree(vbuf);
  return 0;
}
