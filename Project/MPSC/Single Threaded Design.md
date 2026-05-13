# Single Threaded Design

## Functions

### Pop()

```c++
      if (int_tail != int_head){
                std::cout << "pop" << std::endl;
                DataType popped = int_buffer[int_head & (capacity_ - 1)];
                int_buffer[int_head & (capacity_ - 1)] = 0;
                int_head++;

                return popped;
            }
```

### Push()
```cpp
    // if(int_tail - int_head < capacity_){
    //     std::cout << "push" << std::endl;
    //     int_buffer[int_tail & (capacity_ - 1)] = Data;
    //     int_tail = int_tail + 1;
    // }
```