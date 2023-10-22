
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
#include <stdint.h>
#include <stdlib.h>

#define define_heap_buffer_type(_type_name, _element_type)                     \
	typedef struct                                                             \
	{                                                                          \
		_element_type* data;                                                   \
		uint64_t capacity;                                                     \
		uint64_t count;                                                        \
	} _type_name ## _s;                                                        \
	                                                                           \
	_type_name ## _s _type_name ## _from_parts(                                \
		const uint64_t capacity);                                              \
	                                                                           \
	void _type_name ## _append(                                                \
		_type_name ## _s* const heap_buffer,                                   \
		const _element_type element);                                          \
	                                                                           \
	void _type_name ## _shrink(                                                \
		_type_name ## _s* const heap_buffer);                                  \
	                                                                           \
	_element_type* _type_name ## _at(                                          \
		_type_name ## _s* const heap_buffer,                                   \
		const uint64_t index);                                                 \
	                                                                           \
	_Static_assert(1, "") // NOTE: Left fot ';' support after calling the macro.


#define implement_heap_buffer_type(_type_name, _element_type)                  \
	_type_name ## _s _type_name ## _from_parts(                                \
		const uint64_t capacity)                                               \
	{                                                                          \
		debug_assert(capacity > 0);                                            \
		                                                                       \
		_type_name ## _s heap_buffer = {0};                                    \
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
		_type_name ## _s* const heap_buffer,                                   \
		const _element_type element)                                           \
	{                                                                          \
		debug_assert(heap_buffer != NULL);                                     \
		                                                                       \
		if (heap_buffer->count + 1 >= heap_buffer->capacity)                   \
		{                                                                      \
			const uint64_t new_capacity = (uint64_t)(                          \
				heap_buffer->capacity + (heap_buffer->capacity / 2)            \
			);                                                                 \
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
	void _type_name ## _shrink(                                                \
		_type_name ## _s* const heap_buffer)                                   \
	{                                                                          \
		debug_assert(heap_buffer != NULL);                                     \
		                                                                       \
		if (heap_buffer->count >= heap_buffer->capacity)                       \
		{                                                                      \
			return;                                                            \
		}                                                                      \
		                                                                       \
		heap_buffer->data = (_element_type*)realloc(                           \
			heap_buffer->data, heap_buffer->count * sizeof(_element_type)      \
		);                                                                     \
		                                                                       \
		debug_assert(heap_buffer->data != NULL);                               \
		heap_buffer->capacity = heap_buffer->count;                            \
	}                                                                          \
	                                                                           \
	_element_type* _type_name ## _at(                                          \
		_type_name ## _s* const heap_buffer,                                   \
		const uint64_t index)                                                  \
	{                                                                          \
		debug_assert(heap_buffer != NULL);                                     \
		debug_assert(index < heap_buffer->count);                              \
		return &heap_buffer->data[index];                                      \
	}                                                                          \
	                                                                           \
	_Static_assert(1, "") // NOTE: Left fot ';' support after calling the macro.


#ifndef NDEBUG
#	include <debug.h>
#	include <stddef.h>

#	define debug_assert_heap_buffer(_heap_buffer)                              \
		do                                                                     \
		{                                                                      \
			debug_assert((_heap_buffer).data != NULL);                         \
			debug_assert((_heap_buffer).count <= (_heap_buffer).capacity);     \
		} while (0)
#else
#	define debug_assert_heap_buffer(_heap_buffer)
#endif

#endif
