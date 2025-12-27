// Reallocate pointer p of oldsize to newsize, "size" bytes each.
// Will initialize new additions to 0
// -------------------------------------------------------------------------------------------
void* recalloc(void* ptr, size_t oldsize, size_t newsize, size_t size) {
  // realloc with 0 is not c99
  if(!newsize || !size) {
    free(ptr);
    return NULL;
  }

  // ask for memory
  // realloc and pointer arithmetic can be used for faster performance but it clutters valgrind.
  void* res = calloc(newsize, size);
  if(!res) return NULL;
  
  // copy over
  size_t copyBytes = (newsize < oldsize) ? newsize : oldsize; 
  res = memcpy(res, ptr, copyBytes);
  free(ptr);
  return res;
}


// simple resizeable array of strings.
// -------------------------------------------------------------------------------------------
struct Vec {
  char**  arr;        // array of strings
  size_t* len;        // array of their length
  size_t  cap;        // capacity of vector
};


// make a new vector with capacity of 64
// -------------------------------------------------------------------------------------------
struct Vec vnew() {
  // init it to have a capacity of 64 at the beginning
  struct Vec newVector = {
    .arr = calloc(64, sizeof(char*)),
    .len = calloc(64, sizeof(size_t)),
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
  if(index >= vec->cap) {
    // I want NO memory overhead. I don't care about performance
    int oldcap = vec->cap;
    vec->cap = index+1; 
    vec->arr = recalloc(vec->arr, oldcap, vec->cap, sizeof(char*));
    vec->len = recalloc(vec->len, oldcap, vec->cap, sizeof(size_t));
    if(!vec->arr || !vec->len) return 0; // memory error
  }

  // setup the memory for new element
  vec->arr[index] = recalloc(vec->arr[index], vec->len[index], elen, sizeof(char));
  vec->len[index] = elen;
  if(!elem) vec->arr[index] = NULL; // it is free...
  if(!vec->arr[index]) return 0; // memory error 

  // now copy over the element
  strncpy(vec->arr[index], elem, elen); 
  return 1;
}


// get element from vector
// it's a mutable reference, you can free it if you want. Just set *RET = NULL after
// stores the length in lengthBuffer if not NULL
// returns NULL on buffer overflow
char* vget(struct Vec* vec, size_t index) {
  // sanitize
  if(!vec) return NULL;
  if(vec->cap < index) return NULL;
  // return
  return vec->arr[index];
}
