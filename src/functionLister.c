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
