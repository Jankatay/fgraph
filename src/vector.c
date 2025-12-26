// simple resizeable array of strings.
// -------------------------------------------------------------------------------------------
struct Vec {
  char**  str;        // array of strings
  size_t* strlen;     // array of their length
  int len;            // 0-indexed length of vector, -1 on empty
  size_t  cap;        // capacity of vector
};


// make a new vector with capacity of 64
// -------------------------------------------------------------------------------------------
struct Vec vnew() {
  // init it to have a capacity of 64 at the beginning
  struct Vec newVector = {
    .str = calloc(64, sizeof(char*)),
    .strlen = calloc(64, sizeof(int)),
    .len = -1,
    .cap = 64
  };
  // return
  return newVector;
}


// put a new string "elem(ent)" of len "e(lem(ent))len" 
// automatically resizes, returns 0 on memory error.
// using elem=NULL AND elen=0 frees index.
// -------------------------------------------------------------------------------------------
int vset(size_t elen; struct Vec* vec, size_t index, char elem[elen], size_t elen) {
  // sanitize
  if(!vec) return 1;
  if(index > vec->cap && !elem) return 1;

  // resize 
  if(index > vec->cap) {
    // I want NO memory overhead. I don't care about performance
    size_t oldcap = vec->cap;
    vec->cap = index; 
    vec->str = realloc(vec->str, vec->cap);
    if(!vec->str) return 0; // memory error
    // fill the new memory with zeros
    memset(vec->str + oldcap, 0, (index-oldcap));
  }

  // setup the memory for new element
  vec->str[index] = realloc(vec->str[index], elen);
  if(!elem) return 1; // it is free...
  if(!vec->str[index]) return 0; // memory error 

  // now copy over the element
  strncpy(vec->str[index], elem, elen); 
  return 1;
}


// get element from vector
// it's a mutable reference, you can free it if you want. Just set *RET = NULL after
// stores the length in lengthBuffer if not NULL
// returns NULL on buffer overflow
char* vget(struct Vec* vec, size_t index) {
  // sanitize
  if(!vec) return NULL;
  if(vec->len < index) return NULL;
  // return
  return vec->str[index];
}
