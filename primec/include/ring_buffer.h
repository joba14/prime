
/**
 * @file ring_buffer.h
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-10-21
 */

#ifndef __primec__include__ring_buffer_h__
#define __primec__include__ring_buffer_h__

#include <debug.h>

#include <stddef.h>

#define define_ring_buffer_type(_type_name, _element_type, _capacity)          \
	typedef struct                                                             \
	{                                                                          \
		_element_type data[_capacity];                                         \
		uint64_t head;                                                         \
		uint64_t tail;                                                         \
		uint64_t count;                                                        \
	} _type_name ## _t;                                                        \
	                                                                           \
	void _type_name ## _reset(                                                 \
		_type_name ## _t* const ring_buffer);                                  \
	                                                                           \
	void _type_name ## _reset(                                                 \
		_type_name ## _t* const ring_buffer)                                   \
	{                                                                          \
		debug_assert(ring_buffer != NULL);                                     \
		ring_buffer->head = 0;                                                 \
		ring_buffer->tail = 0;                                                 \
		ring_buffer->count = 0;                                                \
	}                                                                          \
	                                                                           \
	bool _type_name ## _give(                                                  \
		_type_name ## _t* const ring_buffer,                                   \
		const _element_type element);                                          \
	                                                                           \
	bool _type_name ## _give(                                                  \
		_type_name ## _t* const ring_buffer,                                   \
		const _element_type element)                                           \
	{                                                                          \
		debug_assert(ring_buffer != NULL);                                     \
		uint64_t next = ring_buffer->head + 1;                                 \
		                                                                       \
		if (next >= (_capacity))                                               \
		{                                                                      \
			next = 0;                                                          \
		}                                                                      \
		                                                                       \
		if (next == ring_buffer->tail)                                         \
		{                                                                      \
			return false;                                                      \
		}                                                                      \
		                                                                       \
		ring_buffer->data[ring_buffer->head] = element;                        \
		ring_buffer->head = next;                                              \
		ring_buffer->count++;                                                  \
		return true;                                                           \
	}                                                                          \
	                                                                           \
	bool _type_name ## _peek(                                                  \
		_type_name ## _t* const ring_buffer,                                   \
		_element_type* const element);                                         \
	                                                                           \
	bool _type_name ## _peek(                                                  \
		_type_name ## _t* const ring_buffer,                                   \
		_element_type* const element)                                          \
	{                                                                          \
		debug_assert(ring_buffer != NULL);                                     \
		debug_assert(element != NULL);                                         \
		                                                                       \
		if (ring_buffer->head == ring_buffer->tail)                            \
		{                                                                      \
			return false;                                                      \
		}                                                                      \
		                                                                       \
		uint64_t next = ring_buffer->tail + 1;                                 \
		if(next >= (_capacity)) next = 0;                                      \
		                                                                       \
		*element = ring_buffer->data[ring_buffer->tail];                       \
		return true;                                                           \
	}                                                                          \
	                                                                           \
	bool _type_name ## _take(                                                  \
		_type_name ## _t* const ring_buffer,                                   \
		_element_type* const element);                                         \
	                                                                           \
	bool _type_name ## _take(                                                  \
		_type_name ## _t* const ring_buffer,                                   \
		_element_type* const element)                                          \
	{                                                                          \
		debug_assert(ring_buffer != NULL);                                     \
		debug_assert(element != NULL);                                         \
		                                                                       \
		if (ring_buffer->head == ring_buffer->tail)                            \
		{                                                                      \
			return false;                                                      \
		}                                                                      \
		                                                                       \
		uint64_t next = ring_buffer->tail + 1;                                 \
		if(next >= (_capacity)) next = 0;                                      \
		                                                                       \
		*element = ring_buffer->data[ring_buffer->tail];                       \
		ring_buffer->tail = next;                                              \
		ring_buffer->count--;                                                  \
		return true;                                                           \
	}                                                                          \
	                                                                           \
	bool _type_name ## _isEmpty(                                               \
		_type_name ## _t* const ring_buffer);                                  \
	                                                                           \
	bool _type_name ## _isEmpty(                                               \
		_type_name ## _t* const ring_buffer)                                   \
	{                                                                          \
		debug_assert(ring_buffer != NULL);                                     \
		return (ring_buffer->head == ring_buffer->tail);                       \
	}                                                                          \
	                                                                           \
	bool _type_name ## _isFull(                                                \
		_type_name ## _t* const ring_buffer);                                  \
	                                                                           \
	bool _type_name ## _isFull(                                                \
		_type_name ## _t* const ring_buffer)                                   \
	{                                                                          \
		debug_assert(ring_buffer != NULL);                                     \
		uint64_t next = ring_buffer->head + 1;                                 \
		                                                                       \
		if (next >= (_capacity))                                               \
		{                                                                      \
			next = 0;                                                          \
		}                                                                      \
		                                                                       \
		return (next == ring_buffer->tail);                                    \
	}

#endif
