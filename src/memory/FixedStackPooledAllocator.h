#ifndef SP_UTIL_MEMORY_STACK_POOLED_ALLOCATOR_H
#define SP_UTIL_MEMORY_STACK_POOLED_ALLOCATOR_H

/*
 * replace DynamicFixedLinkedList with an LinkedList using this allocator
 * template<typename T>
 * using DynamicFixedLinkedList = sp::LinkedList<T,sp::FixedStackPooledAllocator>;
 * 
 * template<typename T>
 * void ensure_capacity(DynamicFixedLinkedList<T>&,std::size_t);
 */

#endif
