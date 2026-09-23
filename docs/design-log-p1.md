# ECE 309 Project 2 Design Log

## 1. Dynamic Memory Management In the Conversation Container

For the `Conversation` class, we had to build a dynamically resizing array from scratch instead of using `std::vector`. This meant managing heap memory manually and ensuring the program stayed memory leak safe.

**Array Growth**
The array starts with a small default capacity. As we add more messages, it eventually fills up when `size_` equals `capacity_`. When this happens the array needs to be made bigger. Adding a few slots at a time is slow to scale up to bigger strings. If you add one slot at a time for N items, you copy 1 item for the first resize, then 2 items, then 3 items, all the way to N. The total number of copies is the sum of numbers between 1 to N, which equals (N(N-1))/2. This gives the process an $O(N^2)$ time complexity. 

To solve this we double the array's capacity each time. Creating a new larger array and transferring the items still takes $O(N)$ time. However, because we double the size, these reallocations happen less and less often as the array grows. This spreads out the cost making the average time to append a single message $O(1)$. To make our code even faster we use `std::move()` when transferring the existing `Message` objects into the new array. This prevents the program from making unnecessary copies of the strings inside each message.

**The Rule of Five:**
Because our class manages a raw `Message*` pointer, the default C++ copy and move rules are risky. They only copy the address of the pointer, causing crashes like double free errors, where two variables are pointing to the same array. We implemented the Rule of Five to solve this:
1. **Destructor:** Uses `delete[]` to free the heap array when object is destroyed.
2. **Copy Constructor and Assignment:** Allocates a new block of memory and copies each item over, so the two objects are completely independent.
3. **Move Constructor and Assignment:** Transfers ownership without allocating new memory by stealing a pointer from a temporary object. It saves the memory address then sets the old object's pointer to `nullptr`.

## 2. Bounded Memory in the Sentinel Scanner

The second challenge was processing the model's output stream without running out of memory. If we append every input text chunk into one giant string, the memory usage would eventually cause the program to crash during long streams.

To fix this, we added a buffer to the `SentinelScanner`. When new text arrives it is added to the buffer. Once the conversation has ended, the text is outputted and the buffer is cleared.

The main edge case is when the text is split across two separate chunks. For example `<|end` in one chunk and `_conversation|>` in the next. The buffer cannot grow endlessly while waiting to see if the rest of the word arrives.

To solve this, the scanner calculates the longest possible partial match that could exist at the end of the buffer. That length is the size of the sentinel minus 1. 

The scanner extracts these safe characters, outputting and deleting them from the buffer. By constantly trimming the string every time `feed()` is called, the memory footprint stays small no matter if ten words or ten gigabytes are processed.