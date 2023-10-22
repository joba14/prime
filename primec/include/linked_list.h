
/**
 * @file linked_list.h
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-10-22
 */

#ifndef __primec__include__linked_list_h__
#define __primec__include__linked_list_h__

#include <debug.h>

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define define_linked_list_type(_type_name, _element_type)                     \
	typedef struct _type_name ## _node ## _s _type_name ## _node ## _s;        \
	                                                                           \
	struct _type_name ## _node ## _s                                           \
	{                                                                          \
		_element_type data;                                                    \
		_type_name ## _node ## _s* prev;                                       \
		_type_name ## _node ## _s* next;                                       \
	};                                                                         \
	                                                                           \
	typedef struct                                                             \
	{                                                                          \
		_type_name ## _node ## _s* head;                                       \
		_type_name ## _node ## _s* tail;                                       \
		uint64_t count;                                                        \
	} _type_name ## _s;                                                        \
	                                                                           \
	_type_name ## _s _type_name ## _from_parts(                                \
		void);                                                                 \
	                                                                           \
	void _type_name ## _push_head(                                             \
		_type_name ## _s* const linked_list,                                   \
		const _element_type element);                                          \
	                                                                           \
	void _type_name ## _push_tail(                                             \
		_type_name ## _s* const linked_list,                                   \
		const _element_type element);                                          \
	                                                                           \
	_element_type _type_name ## _pop_head(                                     \
		_type_name ## _s* const linked_list);                                  \
	                                                                           \
	_element_type _type_name ## _pop_tail(                                     \
		_type_name ## _s* const linked_list);                                  \
	                                                                           \
	_Static_assert(1, "") // NOTE: Left fot ';' support after calling the macro.


#define implement_linked_list_type(_type_name, _element_type)                  \
	_type_name ## _s _type_name ## _from_parts(                                \
		void)                                                                  \
	{                                                                          \
		_type_name ## _s linked_list = {0};                                    \
		linked_list.head = NULL;                                               \
		linked_list.tail = NULL;                                               \
		linked_list.count = 0;                                                 \
		return linked_list;                                                    \
	}                                                                          \
	                                                                           \
	void _type_name ## _push_head(                                             \
		_type_name ## _s* const linked_list,                                   \
		const _element_type element)                                           \
	{                                                                          \
		debug_assert(linked_list != NULL);                                     \
		                                                                       \
		_type_name ## _node ## _s* node = (_type_name ## _node ## _s*)malloc(  \
			sizeof(_type_name ## _node ## _s));                                \
		debug_assert(node != NULL);                                            \
		                                                                       \
		node->data = element;                                                  \
		node->prev = NULL;                                                     \
		node->next = NULL;                                                     \
		                                                                       \
		if (NULL == linked_list->head)                                         \
		{                                                                      \
			linked_list->head = node;                                          \
			linked_list->tail = node;                                          \
		}                                                                      \
		else                                                                   \
		{                                                                      \
			node->next = linked_list->head;                                    \
			linked_list->head->prev = node;                                    \
			linked_list->head = node;                                          \
		}                                                                      \
		                                                                       \
		linked_list->count++;                                                  \
	}                                                                          \
	                                                                           \
	void _type_name ## _push_tail(                                             \
		_type_name ## _s* const linked_list,                                   \
		const _element_type element)                                           \
	{                                                                          \
		debug_assert(linked_list != NULL);                                     \
		                                                                       \
		_type_name ## _node ## _s* node = (_type_name ## _node ## _s*)malloc(  \
			sizeof(_type_name ## _node ## _s));                                \
		debug_assert(node != NULL);                                            \
		                                                                       \
		node->data = element;                                                  \
		node->prev = NULL;                                                     \
		node->next = NULL;                                                     \
		                                                                       \
		if (NULL == linked_list->tail)                                         \
		{                                                                      \
			linked_list->head = node;                                          \
			linked_list->tail = node;                                          \
		}                                                                      \
		else                                                                   \
		{                                                                      \
			node->prev = linked_list->tail;                                    \
			linked_list->tail->next = node;                                    \
			linked_list->tail = node;                                          \
		}                                                                      \
		                                                                       \
		linked_list->count++;                                                  \
	}                                                                          \
	                                                                           \
	_element_type _type_name ## _pop_head(                                     \
		_type_name ## _s* const linked_list)                                   \
	{                                                                          \
		debug_assert(linked_list != NULL);                                     \
		debug_assert(linked_list->head != NULL);                               \
		                                                                       \
		_type_name ## _node ## _s* node = linked_list->head;                   \
		                                                                       \
		if (linked_list->head == linked_list->tail)                            \
		{                                                                      \
			linked_list->head = NULL;                                          \
			linked_list->head = NULL;                                          \
		}                                                                      \
		else                                                                   \
		{                                                                      \
			linked_list->head = linked_list->head->next;                       \
			linked_list->head->prev = NULL;                                    \
		}                                                                      \
		                                                                       \
		_element_type data = node->data;                                       \
		linked_list->count--;                                                  \
		                                                                       \
		free(node);                                                            \
		return data;                                                           \
	}                                                                          \
	                                                                           \
	_element_type _type_name ## _pop_tail(                                     \
		_type_name ## _s* const linked_list)                                   \
	{                                                                          \
		debug_assert(linked_list != NULL);                                     \
		debug_assert(linked_list->tail != NULL);                               \
		                                                                       \
		_type_name ## _node ## _s* node = linked_list->tail;                   \
		                                                                       \
		if (linked_list->head == linked_list->tail)                            \
		{                                                                      \
			linked_list->head = NULL;                                          \
			linked_list->head = NULL;                                          \
		}                                                                      \
		else                                                                   \
		{                                                                      \
			linked_list->tail = linked_list->tail->prev;                       \
			linked_list->tail->next = NULL;                                    \
		}                                                                      \
		                                                                       \
		_element_type data = node->data;                                       \
		linked_list->count--;                                                  \
		                                                                       \
		free(node);                                                            \
		return data;                                                           \
	}                                                                          \
	                                                                           \
	_Static_assert(1, "") // NOTE: Left fot ';' support after calling the macro.


#ifndef NDEBUG
#	include <debug.h>
#	include <stddef.h>

#	define debug_assert_linked_list(_linked_list)                              \
		do                                                                     \
		{                                                                      \
		} while (0)
#else
#	define debug_assert_linked_list(_linked_list)
#endif

#endif
