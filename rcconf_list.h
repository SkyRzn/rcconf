/*
Copyright (c) 2022 Alexander Ivanov

This is free software, and you are welcome to redistribute it
under certain conditions; see the LICENSE file for details.
*/

#ifndef __RCCONF_LIST_H__
#define __RCCONF_LIST_H__

#include <stddef.h>

struct rcconf_list {
	struct rcconf_list *prev;
	struct rcconf_list *next;
};

#define container_of(ptr, type, member) ({ \
	const typeof( ((type *)0)->member ) *__mptr = (ptr); \
	(type *)( (char *)__mptr - offsetof(type,member)); })

#define rcconf_list_foreach(list, item) \
	for (item = (list)->next; (item) != (list); item = (item)->next)

extern void rcconf_list_init(struct rcconf_list *list);
extern void rcconf_list_append(struct rcconf_list *list, struct rcconf_list *item);
extern void rcconf_list_del(struct rcconf_list *item);

#endif
