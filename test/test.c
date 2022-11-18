#include "../rcconf.h"
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>

#define CHEAT_NO_MATH
#include <cheat.h>
#include <cheats.h>


CHEAT_DECLARE(
	#define TMP_PATH        "build-test/tmp"
	#define RCCONF_PATH     TMP_PATH "/test_rc.conf"
	#define RCCONF_OUT_PATH TMP_PATH "/test_rc_out.conf"
	#define INCORRECT_PATH  TMP_PATH "/test_rc_incorrect.conf"
	#define RCCONF_HEADER   "*** HEADER ***"
)

CHEAT_TEST(rcconf,
	struct rcconf cfg;
	struct rcconf_field *field;
	int res;
	FILE *f;
	char buf[64];
	const char *out = RCCONF_HEADER "\nkey1=\"val1\"\nkey2=\"val2\"\nnew2=\"newval2\"\n";

	(void)(cheat_check); // suppress compiler "unused" error

	mkdir(TMP_PATH, 0777);

	f = fopen(RCCONF_PATH, "w");
	fprintf(f, "key1=\"val1\"\n");
	fprintf(f, "key_inv=val\n");
	fprintf(f, "        key2=\"val2\"           \n");
	fprintf(f, "=\"val\"\n");
	fprintf(f, "=\n");
	fprintf(f, "key=\n");
	fprintf(f, "key=\"\n");
	fprintf(f, "\n");
	fprintf(f, "key3=\"val3\"\n");
	fprintf(f, "############\n");
	fclose(f);

	rcconf_init(NULL);
	rcconf_init(&cfg);

	res = rcconf_set_field(&cfg, "key", "val");
	cheat_assert_int(res, 0);
	res = rcconf_set_field(&cfg, NULL, "val");
	cheat_assert_int(res, -EINVAL);
	res = rcconf_set_field(&cfg, "key", NULL);
	cheat_assert_int(res, -EINVAL);

	RC_CONF_FOREACH_FIELD(&cfg, field) {
		cheat_assert_string(field->key, "key");
		cheat_assert_string(field->val, "val");
	}

	res = rcconf_set_field(&cfg, "key", "VAL");
	cheat_assert_string(cfg.fields.next->val, "VAL");

	field = rcconf_get_field(&cfg, NULL);
	cheat_assert_pointer(field, NULL);

	field = rcconf_get_field(NULL, "key");
	cheat_assert_pointer(field, NULL);

	field = rcconf_get_field(&cfg, "key");
	cheat_assert_not_pointer(field, NULL);
	cheat_assert_string(field->val, "VAL");

	res = rcconf_add_field(&cfg, NULL);
	cheat_assert_int(res, -EINVAL);
	res = rcconf_add_field(NULL, field);
	cheat_assert_int(res, -EINVAL);
	res = rcconf_add_field(&cfg, field);
	cheat_assert_int(res, -EEXIST);

	field = rcconf_make_field(NULL, "VAL");
	cheat_assert_pointer(field, NULL);
	field = rcconf_make_field("key", NULL);
	cheat_assert_pointer(field, NULL);
	field = rcconf_make_field("key2", "val2");
	cheat_assert_not_pointer(field, NULL);

	res = rcconf_load(NULL, RCCONF_PATH);
	cheat_assert_int(res, -EINVAL);
	res = rcconf_load(&cfg, NULL);
	cheat_assert_int(res, -EINVAL);
	res = rcconf_load(&cfg, INCORRECT_PATH);
	cheat_assert_int(res, -ENOENT);
	res = rcconf_load(&cfg, RCCONF_PATH);
	cheat_assert_int(res, 0);

	field = rcconf_get_field(&cfg, "key");
	cheat_assert_pointer(field, NULL);
	field = rcconf_get_field(&cfg, "key1");
	cheat_assert_not_pointer(field, NULL);

	res = rcconf_del_field(NULL, "key3");
	cheat_assert_int(res, -EINVAL);
	res = rcconf_del_field(&cfg, NULL);
	cheat_assert_int(res, -EINVAL);
	res = rcconf_del_field(&cfg, "unknown");
	cheat_assert_int(res, -ENOENT);
	res = rcconf_del_field(&cfg, "key3");
	cheat_assert_int(res, 0);

	res = rcconf_set_field(&cfg, "new", "newval");
	cheat_assert_int(res, 0);

	res = rcconf_save(NULL, RCCONF_OUT_PATH, RCCONF_HEADER);
	cheat_assert_int(res, -EINVAL);
	res = rcconf_save(&cfg, NULL, RCCONF_HEADER);
	cheat_assert_int(res, -EINVAL);
	res = rcconf_save(&cfg, TMP_PATH, RCCONF_HEADER);
	cheat_assert_int(res, -EISDIR);
	res = rcconf_save(&cfg, RCCONF_OUT_PATH, RCCONF_HEADER);
	cheat_assert_int(res, 0);

	rcconf_free_field(NULL);

	res = rcconf_save_fields(NULL, NULL, NULL);
	cheat_assert_int(res, -EINVAL);
	res = rcconf_save_fields(INCORRECT_PATH, NULL, NULL);
	cheat_assert_int(res, -ENOENT);

	rcconf_save_fields(RCCONF_OUT_PATH, RCCONF_HEADER, "new", NULL, "new2", "newval2", NULL);

	f = fopen(RCCONF_OUT_PATH, "r");
	fread(buf, sizeof(buf), 1, f);
	fclose(f);

	cheat_assert_int(strcmp(buf, out), 0);

	rcconf_free(&cfg);
	rcconf_free(NULL);
)
