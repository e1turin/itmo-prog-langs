#define TEST_SMART_MMAP

//#include "../../src/mem.h"
//#include "../src/test_utils.h"
//#include "mem_internals.h"
#include "test.h"

#include <inttypes.h>
#include <assert.h>
#include <sys/mman.h>

const size_t HEAP_SIZE = (10 * 1024); //todo: array of different sizes
#define OUTPUT stderr
#define TEST_NAME(_name) fprintf(OUTPUT, "\nRunning test: " #_name "\n")

static void heap_free (void *heap)
{
  munmap (heap, size_from_capacity ((block_capacity){HEAP_SIZE}).bytes);
}

DEFINE_MMAP_IMPL(successful_basic_malloc)
{
  base_mmap_checks (addr, length, prot, flags, fd, offset);
  return mmap (addr, length, prot, flags, fd, offset);
}

/* Обычное успешное выделение памяти. */
DEFINE_TEST(successful_basic_malloc)
{
  TEST_NAME(successful_basic_malloc);
  const size_t BLOCK_1_SIZE = 2000;
  current_mmap_impl = MMAP_IMPL(successful_basic_malloc);

  void *const heap = heap_init (HEAP_SIZE);
  assert (heap != NULL);
  debug_heap (OUTPUT, heap);

  void *const ptr = _malloc (BLOCK_1_SIZE);
  assert (ptr != NULL);

  debug_heap (OUTPUT, heap);
  assert (((struct block_header *)heap)->capacity.bytes >= BLOCK_1_SIZE);

  _free (ptr);
  debug_heap (OUTPUT, heap);
  heap_free (heap);
}

/* Освобождение одного блока из нескольких выделенных. */
DEFINE_TEST(free_single_block_from_many)
{
  TEST_NAME(free_single_block_from_many);
  const size_t BLOCK_1_SIZE = 2000;
  const size_t BLOCK_2_SIZE = 2000;
  const size_t BLOCK_3_SIZE = 2000;

  void *const heap = heap_init (HEAP_SIZE);
  assert(heap != NULL);
  debug_heap (OUTPUT, heap);

  void *const block_1 = _malloc (BLOCK_1_SIZE);
  void *const block_2 = _malloc (BLOCK_2_SIZE);
  void *const block_3 = _malloc (BLOCK_3_SIZE);
  struct block_header *const block_header_1 = block_get_header (block_1);
  struct block_header *const block_header_2 = block_get_header (block_2);
  struct block_header *const block_header_3 = block_get_header (block_3);

  assert (block_1 != NULL && block_header_1->capacity.bytes >= BLOCK_1_SIZE);
  assert (block_2 != NULL && block_header_2->capacity.bytes >= BLOCK_2_SIZE);
  assert (block_3 != NULL && block_header_3->capacity.bytes >= BLOCK_3_SIZE);
  debug_heap (OUTPUT, heap);

  _free (block_2); // 2nd block could not be merged with occupied 3rd
  assert(block_header_1->is_free == false);
  assert(block_header_2->is_free == true);
  assert(block_header_3->is_free == false);
  debug_heap (OUTPUT, heap);

  _free (block_3); // 3rd block must be just freed
  assert(block_header_1->is_free == false);
  assert(block_header_2->is_free == true);
  assert(block_header_3->is_free == true);
  debug_heap (OUTPUT, heap); // only 1st block must be left

  heap_free (heap);

}

/* Освобождение двух блоков из нескольких выделенных. */
DEFINE_TEST(free_two_blocks_from_many)
{
  TEST_NAME(free_two_blocks_from_many);
  const size_t BLOCK_1_SIZE = 2000;
  const size_t BLOCK_2_SIZE = 2000;
  const size_t BLOCK_3_SIZE = 2000;
  const size_t BLOCK_4_SIZE = 2000;

  void *heap = heap_init (HEAP_SIZE);
  assert(heap != NULL);
  debug_heap (OUTPUT, heap);

  void *const block_1 = _malloc (BLOCK_1_SIZE);
  void *const block_2 = _malloc (BLOCK_2_SIZE);
  void *const block_3 = _malloc (BLOCK_3_SIZE);
  void *const block_4 = _malloc (BLOCK_4_SIZE);
  struct block_header *const block_header_1 = block_get_header (block_1);
  struct block_header *const block_header_2 = block_get_header (block_2);
  struct block_header *const block_header_3 = block_get_header (block_3);
  struct block_header *const block_header_4 = block_get_header (block_4);

  assert (block_1 != NULL && block_header_1->capacity.bytes >= BLOCK_1_SIZE);
  assert (block_2 != NULL && block_header_2->capacity.bytes >= BLOCK_2_SIZE);
  assert (block_3 != NULL && block_header_3->capacity.bytes >= BLOCK_3_SIZE);
  assert (block_4 != NULL && block_header_3->capacity.bytes >= BLOCK_4_SIZE);
  debug_heap (OUTPUT, heap);

  _free (block_3); // 3rd block can't be merged with occupied 4th block
  assert (block_header_1->is_free == false);
  assert (block_header_2->is_free == false);
  assert (block_header_3->is_free == true);

  _free (block_2); // 2nd block must be merged with 3rd block
  debug_heap (OUTPUT, heap);
  assert (block_header_1->is_free == false);
  assert (block_header_2->is_free == true);
  assert (block_header_3->is_free == true);

  debug ("2nd block and 3rd block are freed\n");
  assert ((block_header_2->contents + BLOCK_2_SIZE + size_from_capacity ((block_capacity){BLOCK_3_SIZE}).bytes)
		  == (uint8_t *)block_header_4);
  debug_heap (OUTPUT, heap); // should be 2 blocks: 1st and 4th in a row

  heap_free (heap);
}

/* Память закончилась, новый регион памяти расширяет старый. */
DEFINE_TEST(memory_ran_out_and_new_region_extends_previous)
{
  TEST_NAME(memory_ran_out_and_new_region_extends_previous);
  const size_t BLOCK_1_SIZE = (HEAP_SIZE + 10); //size bigger than max heap_size
  const size_t BLOCK_2_SIZE = 2000;

  void *const heap = heap_init (HEAP_SIZE);
  assert(heap != NULL);
  debug_heap (OUTPUT, heap);

  fprintf (stderr, "\n%zu\n", BLOCK_1_SIZE);
  void *const block_1 = _malloc (BLOCK_1_SIZE);
  void *const block_2 = _malloc (BLOCK_2_SIZE);
  struct block_header *const block_header_1 = block_get_header (block_1);
  struct block_header *const block_header_2 = block_get_header (block_2);
  debug_heap (OUTPUT, heap);

  assert (block_1 != NULL && block_header_1->capacity.bytes >= BLOCK_1_SIZE);
  assert (block_2 != NULL && block_header_2->capacity.bytes >= BLOCK_2_SIZE);
  assert (block_header_1->next == block_header_2); // after heap extension 2nd block must start after 1st block
  debug_heap (OUTPUT, heap);

  heap_free (heap);
}

DEFINE_TEST(memory_ran_out_and_old_region_is_not_extendable)
{
  TEST_NAME(memory_ran_out_and_old_region_is_not_extendable);
  const size_t BLOCK_1_SIZE = 4096;
  const size_t BLOCK_2_SIZE = 6000;

  void *const heap = heap_init (HEAP_SIZE);
  assert(heap != NULL);
  debug_heap (OUTPUT, heap);

  void *const block_1 = _malloc (BLOCK_1_SIZE);
  struct block_header *const block_header_1 = block_get_header (block_1);
  assert (block_1 != NULL && block_header_1->capacity.bytes >= BLOCK_1_SIZE);
  debug_heap (OUTPUT, heap);


#undef HEAP_START
#define HEAP_START ((void*)0x04045000)
  void *const barrier = heap_init (10);
  /*mmap (heap_end, REGION_MIN_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_FIXED *//*| MAP_ANONYMOUS*//*, -1, 0);*/
#undef HEAP_START
#define HEAP_START ((void*)0x04040000) // value as in mem.h
  assert(barrier != MAP_FAILED);

  void *const block_2 = _malloc (BLOCK_2_SIZE);
  struct block_header *const block_header_2 = block_get_header (block_2);
  assert (block_2 != NULL && block_header_2->capacity.bytes >= BLOCK_2_SIZE);
  assert (block_header_1->next == block_header_2);
  debug_heap (OUTPUT, heap);

  heap_free (heap);
  munmap (barrier, REGION_MIN_SIZE);
  munmap ((void *)block_header_2, size_from_capacity ((block_capacity){BLOCK_2_SIZE}).bytes);
}

DEFINE_TEST_GROUP(free)
	{
		TEST_IN_GROUP(free_single_block_from_many),
		TEST_IN_GROUP(free_two_blocks_from_many)
	};

DEFINE_TEST_GROUP(region)
	{
		TEST_IN_GROUP(memory_ran_out_and_new_region_extends_previous),
		TEST_IN_GROUP(memory_ran_out_and_old_region_is_not_extendable)
	};

int main ()
{
  RUN_SINGLE_TEST(successful_basic_malloc);
  RUN_TEST_GROUP(free);
  RUN_TEST_GROUP(region);
  return 0;
}
