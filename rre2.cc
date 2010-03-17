#include "ruby.h"

#include <re2/re2.h>

#include <string>
#include <new>
using std::nothrow;

using re2::RE2;
using re2::StringPiece;

static VALUE rb_cRRE2, rb_cRRE2MatchData;

/* RRE2MatchData Class */
struct rre2_matchdata {
	StringPiece *groups;
	int n_groups;
};

extern "C" static void
rre2_matchdata_free(rre2_matchdata *m_obj)
{
	if (m_obj) {
		if (m_obj->groups)
			delete[] m_obj->groups;
		free(m_obj);
	}
}

extern "C" static VALUE
rre2_matchdata_alloc(VALUE klass)
{
	rre2_matchdata *m_obj;
	return Data_Make_Struct(klass, struct rre2_matchdata, 0, rre2_matchdata_free, m_obj);
}

extern "C" static VALUE
rre2_matchdata_to_s(VALUE self)
{
	int i;
	struct rre2_matchdata *m_obj = (struct rre2_matchdata *)DATA_PTR(self);
//	printf("matchdata_to_s: %x\n", m_obj);
	for (i = 0; i < m_obj->n_groups; i++) {
		if (m_obj->groups[i] != NULL) {
//			printf("match[%d]: %s\n", i, m_obj->groups[i].data());
			return rb_str_new2(m_obj->groups[i].as_string().data());
		}
	}
	return Qnil;
}

extern "C" static VALUE
rre2_matchdata_to_a(VALUE self)
{
	VALUE ary;
	int i;
	struct rre2_matchdata *m_obj = (struct rre2_matchdata *)DATA_PTR(self);
//	printf("matchdata_to_s: %x\n", m_obj);
	ary = rb_ary_new();
	for (i = 0; i < m_obj->n_groups; i++) {
		if (m_obj->groups[i] != NULL) {
//			printf("match[%d]: %s\n", i, m_obj->groups[i].data());
			rb_ary_push(ary, rb_str_new2(m_obj->groups[i].as_string().data()));
		}
	}
	return ary;
}

/* RRE2 Class */
extern "C" static void
rre2_free(RE2 *re_obj)
{
	if (re_obj)
		delete re_obj;
}

extern "C" static VALUE
rre2_alloc(VALUE klass)
{
	RE2* re2_obj = NULL;
	return Data_Wrap_Struct(klass, NULL, rre2_free, re2_obj);
}

extern "C" static VALUE
rre2_init(VALUE self, VALUE rpattern)
{
	RE2 *re_obj;

#if 0
	Check_Type(rpattern, T_STRING);
	printf("rre2_init: ruby pattern=%s\n", RSTRING(rpattern)->ptr);
	DATA_PTR(self) = re_obj = new(nothrow) RE2(StringPiece(RSTRING(rpattern)->ptr, RSTRING(rpattern)->len));
#else
	char *pattern = StringValuePtr(rpattern);
//	printf("rre2_init: ruby pattern=%s\n", pattern);
	DATA_PTR(self) = re_obj = new(nothrow) RE2(StringPiece(pattern, strlen(pattern)));
#endif

	if (re_obj == NULL)
		rb_raise(rb_eRuntimeError, "failed to initialize RE2");

	if (!re_obj->ok()) {
		long code = (long)re_obj->error_code();
		const std::string& msg = re_obj->error();
		rb_raise(rb_eRuntimeError, "failed to initialize RE2: %d:%s", code, msg.data());
	}

	return self;
}

extern "C" static VALUE
rre2_match(int argc, VALUE *argv, VALUE self)
{
	RE2 *re_obj = (RE2 *)DATA_PTR(self);
	struct rre2_matchdata *m_obj;
	VALUE result, str, initpos;
	long pos;
	char *data;
	int ret, n_groups = 0;
	StringPiece *groups = NULL;

	if (rb_scan_args(argc, argv, "11", &str, &initpos) == 2) {
		pos = NUM2LONG(initpos);
	} else {
		pos = 0;
	}

    n_groups = re_obj->NumberOfCapturingGroups() + 1;
    groups = new(nothrow) StringPiece[n_groups];

	if (groups == NULL)
		rb_raise(rb_eNoMemError, "failed to init RRE2MatchData");

	// FIXME: should not be using StringValuePtr
	data = StringValuePtr(str);
	if (pos >= strlen(data))
		rb_raise(rb_eRuntimeError, "invalid position specified");

//	printf("Running match on: %s\n", StringPiece(data+pos, strlen(data)-pos).data());

	// FIXME: support different kind of matching
	ret = re_obj->Match(
		StringPiece(data+pos, strlen(data)-pos),
		0,
		RE2::UNANCHORED,
		groups,
		n_groups
	);
	
//	printf("Match returned: %d\n", ret);
	
	if (ret != 1) {
		delete[] groups;
		return Qnil;
	}

	// FIXME: check return value?
    result = rre2_matchdata_alloc(rb_cRRE2MatchData); //reg_match_pos(re, &str, pos);

	m_obj = (struct rre2_matchdata *)DATA_PTR(result);
	m_obj->groups = groups;
	m_obj->n_groups = n_groups;

	if (!NIL_P(result) && rb_block_given_p()) {
		return rb_yield(result);
	}

	return result;
}

extern "C" static VALUE
rre2_inspect(VALUE self)
{
	RE2 *re_obj = (RE2 *)DATA_PTR(self);
	const std::string& msg = re_obj->pattern();
	return rb_str_new2(msg.data());
}

extern "C" static VALUE
rre2_program_size(VALUE self)
{
	RE2 *re_obj = (RE2 *)DATA_PTR(self);
	return rb_int_new(re_obj->ProgramSize());
}

extern "C" void
Init_rre2()
{
	rb_cRRE2 = rb_define_class("RRE2", rb_cObject);
	rb_define_alloc_func(rb_cRRE2, rre2_alloc);
	rb_define_method(rb_cRRE2, "initialize", (VALUE (*)(...))rre2_init, 1);
    rb_define_method(rb_cRRE2, "match", (VALUE (*)(...))rre2_match, -1);
    rb_define_method(rb_cRRE2, "to_s", (VALUE (*)(...))rre2_inspect, 0);
    rb_define_method(rb_cRRE2, "inspect", (VALUE (*)(...))rre2_inspect, 0);
    rb_define_method(rb_cRRE2, "source", (VALUE (*)(...))rre2_inspect, 0);

	rb_define_method(rb_cRRE2, "program_size", (VALUE (*)(...))rre2_program_size, 0);

	rb_cRRE2MatchData = rb_define_class("RRE2MatchData", rb_cObject);
	rb_define_alloc_func(rb_cRRE2MatchData, rre2_matchdata_alloc);
	rb_undef_method(CLASS_OF(rb_cRRE2MatchData), "new");

	rb_define_method(rb_cRRE2MatchData, "to_s", (VALUE (*)(...))rre2_matchdata_to_s, 0);
	rb_define_method(rb_cRRE2MatchData, "to_a", (VALUE (*)(...))rre2_matchdata_to_a, 0);
};
