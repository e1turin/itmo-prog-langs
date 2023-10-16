#include <stdio.h>
#define print_var(x) printf(#x " is %d", x );
#define y 1337


int main(){
  int x = 42;
  print_var(x);
  print_var(216);
  print_var(y);
  return 0;
}
