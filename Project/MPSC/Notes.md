# Errors:

- Stack Smashing:
I set the capacity to 1024, when the buffer I was using had the capacity of 4. This caused a "segmentation fault" but on the stack called "stack smashing". Its an out-of-bound error. 