in x86-64/Arm system  
- stack address always > heap address
- new stack value always lower than old stack value.

```
#include <stdio.h>
bool is_in_heap(void* ptr) {
    int test_var;
    if (static_cast<int*>(ptr) < &test_var) {
        return true;
    }
    return false;
}

int main(int argc, char* argv[]) {
    int a = 0;
    if (is_in_heap(&a)) {
        printf("is in heap\n");
    } else {
        printf("not in heap\n");
    }
    
    return 0;
}
```
