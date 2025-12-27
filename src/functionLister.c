#include "base.c"
#include "vector.c"


// Check tests/lister.c is being listed correctly.
// -------------------------------------------------------------------------------------------
int main() {
  // get the first function
  FILE* fp = fopen("tests/lister.c", "r");
  char buf[BUFSIZE] = "";
  int buflen = 0;

  // for each token
  enum Token tok = TOK_ERR;

  // vector tests
  struct Vec vec = vnew();
  vset(&vec, 0, "hi", 3);
  vset(&vec, 109, "lmao", 5);
  vset(&vec, 109, "h", 2);
  
  for(int i = 0; i < vec.cap; i++) {
    if(vec.arr[i]) printf("%d: %s\n", i, vec.arr[i]);
    vset(&vec, i, NULL, 0);
  }
  free(*vec.arr);
  free(vec.arr);
  free(vec.len);
/*
  while((tok = skip(fp, OP_PAREN, TOK_EOF, buf, &buflen))) {
    // check the token
    if(tok == TOK_ERR) {
      fprintf(stderr, "Token error\n");
      break;
    }

    // print the token
    printf("%s\n", buf);
  }
*/

  // success
  fclose(fp);
  return 0;
}
