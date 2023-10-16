/* generic_list.c */

#include <inttypes.h>
#include <stdint.h>

#define print(x)                             \
  _Generic((x), int64_t                      \
           : printf("%" PRId64, (x)), double \
           : printf("%lf", (x)), default     \
           : error("Unsupported operation"));

#define DEFINE_LIST(type)                                         \
  struct list_##type {                                            \
    type value;                                                   \
    struct list_##type *next;                                     \
  };                                                              \
                                                                  \
  void list_##type##_push(struct list_##type *list, type value) { \
    struct list_##type *next =                                    \
        (struct list_##type *)malloc(sizeof(struct list_##type)); \
    next->value = value;                                          \
                                                                  \
    for (; list->next != 0; list = list->next)                    \
      ;                                                           \
    list->next = next;                                            \
  }                                                               \
                                                                  \
  void list_##type##_print(struct list_##type *list) {            \
    printf("{");                                                  \
    for (; list != 0; list = list->next) {                        \
      _Generic((list->value), int64_t                             \
               : printf("%" PRId64, (list->value)), double        \
               : printf("%lf", (list->value)), default            \
               : error("Unsupported operation"));                 \
      printf(", ");                                               \
    }                                                             \
    printf("}");                                                  \
  }
/*list_##type##_print_value(list->value); */
/*
   void list_int64_t_print_value(int64_t value) { printf("%" PRId64, value); }
   void list_double_print_value(double value) { printf("%lf", value); }
   */

DEFINE_LIST(int64_t);
DEFINE_LIST(double);

int main() {
  struct list_int64_t *int_list =
      (struct list_int64_t *)malloc(sizeof(struct list_int64_t));
  list_int64_t_push(int_list, 1);
  list_int64_t_push(int_list, 2);
  list_int64_t_push(int_list, 3);
  list_int64_t_push(int_list, 4);
  list_int64_t_push(int_list, 5);

  list_int64_t_print(int_list);

  puts("");

  /*
     struct list_double *double_list =
     (struct list_double *)malloc(sizeof(struct list_double));
     list_double_push(double_list, 1);
     list_double_push(double_list, 2);
     list_double_push(double_list, 5);

     list_double_print(double_list);
     puts("");

     return 0;
     */
}
