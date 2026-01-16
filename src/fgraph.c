#include "flist.c"
#include "file.c"
#include <ftw.h>
#include <unistd.h>


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
    .map = calloc(usermade.cap, sizeof(struct Vec))
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


// go through a file "codepath" and print graphviz format dependencies to "dest"
// needs a vector of usermade function names "names"
// -------------------------------------------------------------------------------------------
void gprint(FILE* dest, char* codepath, struct Vec names) {
  // sanitize
  if(!codepath || !names.cap || !dest) return;

  // open the file
  FILE* fp = fopen(codepath, "r");
  if(!fp) return;

  // get the function calls calls and print them
  struct Graph temp = fcalls(fp, names, NULL);
  if(!temp.map || !temp.names.cap) {
    if(temp.map) free(temp.map);
    fclose(fp);
    return; 
  }

  // for each function
  for(int i = 0; i < temp.names.cap; i++) {
    // get the function name
    char* name = temp.names.arr[i];
    if(!name) continue;

    // get the dependencies
    struct Vec map = temp.map[i];
    if(!map.cap) continue;

    // for each dependency
    for(int j = 0; j < map.cap; j++) {
      // print the digraph format "dependency -> function name" for graphviz
      char* depends = map.arr[j];
      if(!depends) continue;
      fprintf(dest, "\t%s -> %s\n", depends, name);
      // empty the mapping
      free(depends);
    }

    // empty the graph
    free(map.arr);
    free(map.len);
  }

  // free and exit
  vempty(temp.names);
  vfree(temp.names);
  free(temp.map);
  fclose(fp);
}


// read files from arguments print all functions along with their dependencies into stdout
// -------------------------------------------------------------------------------------------
int main(int argc, char* argv[]) {
  // init commandline options
  FILE* output = stdout;
  
  // get path and optional output
  char opt = 0;
  int specificDirs = 0;
  while((opt = getopt(argc, argv, "-ho:")) != -1) {
    // printing usage
    if(opt == 'h') {
      fprintf(stderr, "Usage: %s [-o output.dot] [filenames]");
      return opt == 'h';
    }
    
    // setting output
    if(opt == 'o') {
      char* filename = optarg;
      if((output = fopen(filename, "w+"))) continue;
      fprintf(stderr, "Could not open %s\n", filename);
      return 1;
    }

    // by default it's an input path 
    nftw(optarg, files, 20, FTW_DEPTH | FTW_PHYS);
    specificDirs = 1;
  }

  // if paths were not specified, run on current dir
  if(!specificDirs) nftw(".", files, 20, FTW_DEPTH | FTW_PHYS);
  if(!finfo.arr) return 0;

  // for each path information
  struct Vec vbuf = {};
  for(int i = 0; i < finfo.cap; i++) {
    // get name
    char* name = finfo.arr[i];
    if(!name) continue;
    // setup a list of usermade functions
    ladd(&vbuf, name);
  }

  // graphviz header
  fprintf(output, "digraph {\n");

  // then go through those files with the list in your hand
  for(int i = 0; i < finfo.cap; i++) {
    // get name
    char* name = finfo.arr[i];
    if(!name) continue;
    // print graphviz format using that list
    gprint(output, name, vbuf);
  }

  // graphviz footer
  fprintf(output, "}\n");

  // free and exit
  vempty(vbuf);
  vfree(vbuf);
  vempty(finfo);
  vfree(finfo);
  fclose(output);
  return 0;
}
