/*
Copyright (c) 2022 Alexander Ivanov

This is free software, and you are welcome to redistribute it
under certain conditions; see the LICENSE file for details.
*/

#include "rcconf.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/types.h>


static struct rcconf_field *parse_line(char *line);
static void strip(char *str);
static bool is_space(char c);


int rcconf_save_fields(const char *path, const char *header, ...)
{
	struct rcconf cfg;
	va_list args;
	const char *key, *val;
	int res;

	if ((!path)) {
		return -EINVAL;
	}

	rcconf_init(&cfg);

	res = rcconf_load(&cfg, path);
	if (res != 0) {
		return res;
	}

	va_start(args, header);

	do {
		key = va_arg(args, const char *);
		if (key == NULL) {
			break;
		}

		val = va_arg(args, const char *);
		if (val == NULL) {
			res = rcconf_del_field(&cfg, key);
			if (res != 0 && res != -ENOENT) {
				goto err;
			}
		} else {
			res = rcconf_set_field(&cfg, key, val);
			if (res != 0) {
				goto err;
			}
		}

	} while (key != NULL);

	va_end(args);

	res = rcconf_save(&cfg, path, header);

err:
	rcconf_free(&cfg);

	return res;
}

void rcconf_init(struct rcconf *cfg)
{
	if (!cfg) {
		return;
	}
	cfg->fields.next = &cfg->fields;
	cfg->fields.prev = &cfg->fields;
}

void rcconf_free(struct rcconf *cfg)
{
	struct rcconf_field *field, *next;

	if (!cfg) {
		return;
	}

	next = NULL;
	for (field = cfg->fields.next; field != &cfg->fields; field = next) {
		next = field->next;
		rcconf_free_field(field);
	}

	cfg->fields.prev = &cfg->fields;
	cfg->fields.next = &cfg->fields;
}

int rcconf_load(struct rcconf *cfg, const char *path)
{
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	struct rcconf_field *field;

	if ((!cfg) || (!path)) {
		return -EINVAL;
	}

	rcconf_free(cfg);

	fp = fopen(path, "r");
	if (!fp)
		return -errno;

	while ((read = getline(&line, &len, fp)) != -1) {
		field = parse_line(line);
		rcconf_add_field(cfg, field);
	}
	free(line);
	fclose(fp);

	return 0;
}

int rcconf_save(struct rcconf *cfg, const char *path, const char *header)
{
	FILE *fp;
	struct rcconf_field *field;

	if ((!cfg) || (!path)) {
		return -EINVAL;
	}

	fp = fopen(path, "w+");
	if (!fp)
		return -errno;

	if (header) {
		fprintf(fp, "%s\n", header);
	}

	RC_CONF_FOREACH_FIELD(cfg, field) {
		fprintf(fp, "%s=\"%s\"\n", field->key, field->val);
	}
	fclose(fp);

	return 0;
}


struct rcconf_field *rcconf_get_field(struct rcconf *cfg, const char *key)
{
	struct rcconf_field *field;

	if ((!cfg) || (!key)) {
		return NULL;
	}

	RC_CONF_FOREACH_FIELD(cfg, field) {
		if (strcmp(field->key, key) == 0) {
			return field;
		}
	}

	return NULL;
}

int rcconf_set_field(struct rcconf *cfg, const char *key, const char *val)
{
	struct rcconf_field *field;
	char *tmp;

	if ((!cfg) || (!key) || (!val)) {
		return -EINVAL;
	}

	field = rcconf_get_field(cfg, key);
	if (field) {
		tmp = strdup(val);
		if (!tmp) {
			return -ENOMEM;
		}

		free(field->val);
		field->val = tmp;
		return 0;
	}

	field = rcconf_make_field(key, val);
	if (!field) {
		return -errno;
	}

	return rcconf_add_field(cfg, field);
}

int rcconf_del_field(struct rcconf *cfg, const char *key)
{
	struct rcconf_field *field;

	if ((!cfg) || (!key)) {
		return -EINVAL;
	}

	field = rcconf_get_field(cfg, key);
	if (!field) {
		return -ENOENT;
	}

	field->next->prev = field->prev;
	field->prev->next = field->next;
	return 0;
}

int rcconf_add_field(struct rcconf *cfg, struct rcconf_field *field)
{
	struct rcconf_field *check;

	if ((!cfg) || (!field)) {
		return -EINVAL;
	}

	check = rcconf_get_field(cfg, field->key);
	if (check) {
		return -EEXIST;
	}

	field->prev = cfg->fields.prev;
	field->next = &cfg->fields;
	cfg->fields.prev->next = field;
	cfg->fields.prev = field;

	return 0;
}

struct rcconf_field *rcconf_make_field(const char *key, const char *val)
{
	struct rcconf_field *field;

	if ((!key) || (!val)) {
		return NULL;
	}

	field = malloc(sizeof(*field));
	if (!field) {
		errno = -ENOMEM;
		return NULL;
	}

	field->next = NULL;
	field->key = strdup(key);
	field->val = strdup(val);

	if ((!field->key) || (!field->val)) {
		rcconf_free_field(field);
		errno = -ENOMEM;
		return NULL;
	}

	return field;
}

void rcconf_free_field(struct rcconf_field *field)
{
	if (!field) {
		return;
	}
	free(field->key);
	free(field->val);
	free(field);
}

static struct rcconf_field *parse_line(char *line)
{
	char *p, *key, *val, *end;

	p = strchr(line, '=');
	if (!p) {
		return NULL;
	}

	*p = '\0';
	key = line;
	val = p + 1;

	strip(key);
	if (strlen(key) == 0) {
		return NULL;
	}

	strip(val);
	if (strlen(val) < 2) {
		return NULL;
	}

	end = val + strlen(val) - 1;
	if (*val != '"' || *end != '"') {
		return NULL;
	}
	val++;
	*end = '\0';

	return rcconf_make_field(key, val);
}

static void strip(char *str)
{
	char *beg, *end;

	beg = str;
	end = beg + strlen(str);

	for (; beg != end && is_space(*beg); beg++);
	for (; beg != end && is_space(*end); end--);

	memmove(str, beg, end - beg + 1);
	str[end - beg + 1] = '\0';
}

static bool is_space(char c)
{
	return (c == ' ' || c == '\t' || c == '\n' || c == '\0');
}
