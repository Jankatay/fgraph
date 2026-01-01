#include "flist.c"


// map a function to functions it uses
struct Map {
  char* fname;       // function's name
  int flen;          // length of fname
  struct Vec fdep;   // array of dependencies
};


// starting from inside a function body, list all the functions in list of "usermade" ones.
// sets status to 0 on error and 1 on success, nullable.
// -------------------------------------------------------------------------------------------


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
