
/**
 * @file heap_buffer.h
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-10-21
 */

#ifndef __primec__include__heap_buffer_h__
#define __primec__include__heap_buffer_h__

#include <debug.h>

#include <stddef.h>

#define define_heap_buffer_type(_type_name, _element_type)                     \
	typedef struct                                                             \
	{                                                                          \
		_element_type* data;                                                   \
		uint64_t capacity;                                                     \
		uint64_t count;                                                        \
	} _type_name ## _t;                                                        \
	                                                                           \
	_type_name ## _t _type_name ## _from_parts(                                \
		const uint64_t capacity);                                              \
	                                                                           \
	_type_name ## _t _type_name ## _from_parts(                                \
		const uint64_t capacity)                                               \
	{                                                                          \
		debug_assert(capacity > 0);                                            \
		                                                                       \
		_type_name ## _t heap_buffer;                                          \
		                                                                       \
		heap_buffer.data = (_element_type*)malloc(                             \
			capacity * sizeof(_element_type));                                 \
		debug_assert(heap_buffer.data != NULL);                                \
		                                                                       \
		heap_buffer.capacity = capacity;                                       \
		heap_buffer.count = 0;                                                 \
		return heap_buffer;                                                    \
	}                                                                          \
	                                                                           \
	void _type_name ## _append(                                                \
		_type_name ## _t* const heap_buffer,                                   \
		const _element_type element);                                          \
	                                                                           \
	void _type_name ## _append(                                                \
		_type_name ## _t* const heap_buffer,                                   \
		const _element_type element)                                           \
	{                                                                          \
		debug_assert(heap_buffer != NULL);                                     \
		                                                                       \
		if (heap_buffer->count + 1 >= heap_buffer->capacity)                   \
		{                                                                      \
			const uint64_t new_capacity = (uint64_t)(heap_buffer->capacity +   \
				(heap_buffer->capacity / 2));                                  \
			                                                                   \
			heap_buffer->data = (_element_type*)realloc(                       \
				heap_buffer->data, new_capacity * sizeof(_element_type)        \
			);                                                                 \
			                                                                   \
			debug_assert(heap_buffer->data != NULL);                           \
			heap_buffer->capacity = new_capacity;                              \
		}                                                                      \
		                                                                       \
		heap_buffer->data[heap_buffer->count++] = element;                     \
	}                                                                          \
	                                                                           \
	_element_type* _type_name ## _at(                                          \
		_type_name ## _t* const heap_buffer,                                   \
		const uint64_t index);                                                 \
	                                                                           \
	_element_type* _type_name ## _at(                                          \
		_type_name ## _t* const heap_buffer,                                   \
		const uint64_t index)                                                  \
	{                                                                          \
		debug_assert(heap_buffer != NULL);                                     \
		debug_assert(index < heap_buffer->count);                              \
		return &heap_buffer->data[index];                                      \
	}

#endif
