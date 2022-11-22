/*
Copyright (c) 2022 Alexander Ivanov

This is free software, and you are welcome to redistribute it
under certain conditions; see the LICENSE file for details.
*/

#ifndef __RCCONF_H__
#define __RCCONF_H__

struct rcconf_field {
	char *key;
	char *val;
	struct rcconf_field *next;
	struct rcconf_field *prev;
};

struct rcconf {
	struct rcconf_field fields;
};

#define RC_CONF_FOREACH_FIELD(cfg, field) \
	for (field = (cfg)->fields.next; (field) != &(cfg)->fields; field = (field)->next)

/* You can call this function with any number of key/val. You must
 * terminate argumnets with NULL. There should be an even number of
 * arguments, excluding terminating NULL.
 * Also you can remove some fields by setting NULL for val argument.
 * Note that you must add teminating NULL in any case.
 *
 * Example:
 *     rcconf_save_fields("/etc/rc.conf", "header",
 *                        "key1", "val1, "key2", "val2", "rm_key3", NULL, NULL);
 *                        |_______ add key/val _______|  |__rm key/val_|  |_teminator
 */
extern int rcconf_save_fields(const char *path, const char *header, ...);

extern void rcconf_init(struct rcconf *cfg);
extern void rcconf_free(struct rcconf *cfg);
extern int rcconf_load(struct rcconf *cfg, const char *path);
extern int rcconf_save(struct rcconf *cfg, const char *path, const char *header);
extern struct rcconf_field *rcconf_get_field(struct rcconf *cfg, const char *key);
extern int rcconf_set_field(struct rcconf *cfg, const char *key, const char *val);
extern int rcconf_del_field(struct rcconf *cfg, const char *key);
extern int rcconf_add_field(struct rcconf *cfg, struct rcconf_field *field);
extern struct rcconf_field *rcconf_make_field(const char *key, const char *val);
extern void rcconf_free_field(struct rcconf_field *field);

#endif
