#include "flist.c"
#include "file.c"
#include <ftw.h>


// Graph to map a function to its dependencies.
// -------------------------------------------------------------------------------------------
struct Graph {
  struct Vec names;   // each function name in order
  struct Vec* map;    // mapping of names to dependencies
};


// follow a function from right after start of the curly brackets until the end
// returns a vector of all identifiers that were in the vector "usermade"
// -------------------------------------------------------------------------------------------
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


// go through a file "codepath" and add graph to vector gbuf
// needs a vector of usermade function names "names"
// if *status is 1, will stay 1 and skip the function
// if *status is 0, will put the error status
// if status is not NULL, will ignore.
// example usage :
//         // init
//         struct Graph gbuf = {};
//         int &gerror = 0;
//
//         // process files a, b, c, d
//         gadd("a", &gbuf, &gerror);  
//         gadd("b", &gbuf, &gerror);
//         gadd("c", &gbuf, &gerror);
//         gadd("d", &gbuf, &gerror);
//
//         // return 0 on success and 1 on error
//         return gerror
// -------------------------------------------------------------------------------------------
void gadd(char* codepath, struct Graph* gbuf, struct Vec names, int* status) {
  // make a recycling bin for the status
  int sdump;
  if(!status) status = &sdump;

  // sanitize
  if(!codepath || !gbuf || !names.cap) {
    *status = 1;
    return;
  }

  // open the file
  FILE* fp = fopen(codepath, "r");
  if(!fp) {
    *status = 1;
    return;
  }

  // get the calls
  struct Graph temp = fcalls(fp, names, status);
  // TODO: manage appending fast
  //gbuf.map = recalloc
  
  // append to gbuf
  int oset = gbuf.cap;
  for(int i = 0; i < temp.names.cap; i++) {
    gbuf.names.arr[oset+i] = temp.names.arr[i];
  //
    vset(gbuf.names, gbuf.names.cap, )
    gbuf.names[gbuf.names.cap] = temp.names;
  }

}



// executable form
// -------------------------------------------------------------------------------------------
int main(int argc, char* argv[]) {
  // get files, return if no files found
  for(int i = 1; i < argc; i++) nftw(argv[i], files, 20, FTW_DEPTH | FTW_PHYS);
  if(argc == 1) nftw(".", files, 20, FTW_DEPTH | FTW_PHYS);
  if(!finfo.arr) return 0;

  // for each file information
  struct Vec vbuf = {};
  for(int i = 0; i < finfo.cap; i++) {
    // get name
    char* name = finfo.arr[i];
    if(!name) continue;
    // add to list
    ladd(&vbuf, name);
  }

  for(int i = 0; i < vbuf.cap; i++) {
    if(vbuf.arr[i]) printf("%s\n", vbuf.arr[i]);
  }
    
  return 0;
}
