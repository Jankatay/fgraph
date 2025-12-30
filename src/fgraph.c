#include "flist.c"


// executable form
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
