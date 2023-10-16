/* heap-1.c */

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define HEAP_BLOCKS 16
#define BLOCK_CAPACITY 1024

enum block_status { BLK_FREE = 0, BLK_ONE, BLK_FIRST, BLK_CONT, BLK_LAST };

struct heap {
  struct block {
    char contents[BLOCK_CAPACITY];
  } blocks[HEAP_BLOCKS];
  enum block_status status[HEAP_BLOCKS];
} global_heap = {0};

struct block_id {
  size_t       value;
  bool         valid;
  struct heap* heap;
};

/*  Util function
 *  @param  bid Block identifier
 *  @return Status of the block
 */
static inline enum block_status block_id_get_status(struct block_id bid){
  return bid.heap->status[bid.value];
}

/*  Util function to free number size blocks in given heap from given index  
 *  @param  hp      Ptr to heap where should be freed blocks
 *  @param  from    Index of first block of cleaning region    
 *  @param  size    Amount of blocks to clean
 *  @return Size if OK else 0
 */
size_t heap_free(struct heap* hp, size_t from, size_t size){    //XXX: size or index after last? 
  if (from + size >= HEAP_BLOCKS) return 0;
  if (from < 0) return 0;
  for(size_t i = from; i < from + size; ++i){
    hp->status[i] = BLK_FREE;
  }
  return size;  
}

struct block_id block_id_new(size_t value, struct heap* from) {
  return (struct block_id){.value = value, .valid = true, .heap = from};
}

struct block_id block_id_invalid() {
  return (struct block_id){.valid = false};
}

bool block_id_is_valid(struct block_id bid) {
  return bid.valid && bid.value < HEAP_BLOCKS;
}

/* Find block */

bool block_is_free(struct block_id bid) {
  if (!block_id_is_valid(bid))
    return false;
  return bid.heap->status[bid.value] == BLK_FREE;
}

/* Allocate */

/*  Function to allocate a segment of blocks of a given size
 *  @param  heap    Ptr to heap where should be allocated the segment
 *  @param  size    Size of segment to allocate
 *  @return Identifier of first block of allocated segment
 */
struct block_id block_allocate(struct heap* heap, size_t size) {
  if (size <= 0) return block_id_invalid();

  for (size_t i = 0; i < HEAP_BLOCKS; ++i){
    if (heap->status[i] != BLK_FREE) continue;

    for (size_t j = i; j < HEAP_BLOCKS; ++j) {
      if (heap->status[j] != BLK_FREE) {
        i = j;
        break;
      }
      if (j + 1 != i + size) continue; 

      if (size == 1) {
        heap->status[i] = BLK_ONE;
      } else {
        heap->status[i] = BLK_FIRST;
        for (size_t c = i + 1; c < j; ++c) heap->status[c] = BLK_CONT;
        heap->status[j] = BLK_LAST;
      }
      return block_id_new(i, heap);
    }
  }

  return block_id_invalid();
}

/* Free */

/*  Function frees whole segment which has given bid inside. 
 *  @param  bid Block from a segment which need to be freed (valid block with
 *              status one of [BLK_ONE, BLK_FIRST, BLK_CONT, BLK_LAST])
 */
void block_free_segment_with(struct block_id bid){
  if (block_id_is_valid(bid) && !block_is_free(bid)){
      //fprintf(stdout, "(debug): block_id_is_valid and not free\n");
    if (block_id_get_status(bid) == BLK_ONE) {
      //fprintf(stdout, "(debug): block_id status is BLK_ONE\n");
      bid.heap->status[bid.value] = BLK_FREE;
    } else {

      size_t first = bid.value;
      //fprintf(stdout, "(debug): status[bid.value] = %d in enum\n", bid.heap->status[bid.value]);
      //fprintf(stdout, "(debug): first = %zu //before\n", first);
      for(; first >= 0; --first){   //XXX: bad pattern to substract from size_t
        if(bid.heap->status[first] == BLK_FIRST) break;
      }
      //fprintf(stdout, "(debug): first = %zu //after\n", first);
      //fprintf(stdout, "(debug): status[first] = %d in enum\n", bid.heap->status[first]);

      size_t last = bid.value;
      //fprintf(stdout, "(debug): last = %zu //before \n", last);
      for(; last < HEAP_BLOCKS; ++last){
        if(bid.heap->status[last] == BLK_LAST) break;
      }
      //fprintf(stdout, "(debug): last = %zu //after\n", last);
      //fprintf(stdout, "(debug): status[last] = %d \n", bid.heap->status[last]);

      heap_free(bid.heap, first, last - first + 1); //XXX: bad pattern to substruct from size_t
    }
  }
}

/*  Function frees whole segment which has given bid inside. 
 *  @param  bid     Block from a segment which need to be freed (valid block with
 *                  status one of [BLK_ONE, BLK_FIRST, BLK_CONT, BLK_LAST])
 *  @return Amount of freed blocks in segment
 */
size_t block_free(struct block_id bid){
  if (block_id_is_valid(bid) && !block_is_free(bid)){
    //fprintf(stdout, "(debug): block_id_is_valid and not free\n");
    enum block_status bid_status = block_id_get_status(bid);
    if (bid_status == BLK_ONE) {
      //fprintf(stdout, "(debug): block_id status is BLK_ONE\n");
      bid.heap->status[bid.value] = BLK_FREE;
      return 1;
    } 
    if (bid_status != BLK_FIRST) { 
      //fprintf(stderr, "(debug): Block id must reference to a block with status BLK_FIRST");
      return 0; 
    }
    size_t first = bid.value;
    //fprintf(stdout, "(debug): first = %zu //after\n", first);
    //fprintf(stdout, "(debug): status[first] = %d in enum\n", bid.heap->status[first]);

    size_t last = bid.value;
    bool last_is_not_found = 1;
    //fprintf(stdout, "(debug): last = %zu //before \n", last);
    for(; last < HEAP_BLOCKS; ++last){
      if (bid.heap->status[last] == BLK_LAST) { 
        last_is_not_found = 0; 
        break;
      }
    }
    if (last_is_not_found){
      //fprintf(stderr, "(debug): last is not found");
      return 0;
    }
    
    //fprintf(stdout, "(debug): last = %zu //after\n", last);
    //fprintf(stdout, "(debug): status[last] = %d \n", bid.heap->status[last]);

    return heap_free(bid.heap, first, last - first + 1); //XXX: substruct from size_t
  }
}

/* Printer */
const char* block_repr(struct block_id b) {
  static const char* const repr[] = {
    [BLK_FREE]     = " .",
    [BLK_ONE]      = " *",
    [BLK_FIRST]    = "[=",
    [BLK_LAST]     = "=]",
    [BLK_CONT]     = " ="
  };
  if (b.valid)
    return repr[b.heap->status[b.value]];
  else
    return "INVALID";
}

void block_debug_info(struct block_id b, FILE* f) {
  fprintf(f, "%s", block_repr(b));
}

void block_foreach_printer(struct heap* h, size_t count,
                           void printer(struct block_id, FILE* f), FILE* f) {
  for (size_t c = 0; c < count; c++)
    printer(block_id_new(c, h), f);
}

void heap_debug_info(struct heap* h, FILE* f) {
  block_foreach_printer(h, HEAP_BLOCKS, block_debug_info, f);
  fprintf(f, "\n");
}
/*  -------- */

int main() {
  heap_debug_info(&global_heap, stdout);

  block_allocate(&global_heap, 2);
  heap_debug_info(&global_heap, stdout);

  struct block_id bid = block_allocate(&global_heap, 3);
//  heap_debug_info(&global_heap, stdout);

  block_allocate(&global_heap, 3);
  heap_debug_info(&global_heap, stdout);

  //block_free(bid);
  heap_debug_info(&global_heap, stdout);
  struct block_id bid_2 = block_allocate(&global_heap, 8);
  block_allocate(&global_heap, 8);
  heap_debug_info(&global_heap, stdout);
  return 0;
}
