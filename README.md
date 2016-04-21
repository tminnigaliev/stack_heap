# stack_heap
this project is yet another heap manager.
what made me to write it?
1) Some DSPs doesn't have malloc and free.
2) Many DSPs need blocks of memory aligned by multiple of 4, 8, 16 etc (for example, for DMA purposes). 
Imagine, that standard malloc returned non-aligned block (for example, starting from address 0x87654321) and you need aligned by 4 block.
You can just occupy block with bigger length and ceil 0x87654321 to 0x87654324. But when you call free, you will have to provide original address (0x87654321) so you'll have to store it somewhere. Stack_heap returns blocks with required alignment.
3) Many DSPs has more than one memory, you may want to have more than one heap.
stack_heap allows to create any amount of heaps, providing separate descriptor for each heap. In some cases it may be useful to be able to create a heap inside of block of another heap. stack_heap addresses this request as well.

Documentation is written in Russian.
What may be improved in this heap manager:
- adding option to construct a heap from more than one separate memory blocks.
- refactroring.

in case of any questions please write to programmer78@bk.ru
