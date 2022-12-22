/*
Copyright (c) 2022 Alexander Ivanov

This is free software, and you are welcome to redistribute it
under certain conditions; see the LICENSE file for details.
*/

#include "rcconf_list.h"

void rcconf_list_init(struct rcconf_list *list)
{
	list->prev = list;
	list->next = list;
}

void rcconf_list_append(struct rcconf_list *list, struct rcconf_list *item)
{
	item->prev = list->prev;
	item->next = list;
	list->prev->next = item;
	list->prev = item;
}

void rcconf_list_del(struct rcconf_list *item)
{
	item->prev->next = item->next;
	item->next->prev = item->prev;
}
