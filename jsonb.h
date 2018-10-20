#ifndef JSONB_PARSER_H
#define JSONB_PARSER_H 1

#define JSONB_END_OF_READ -1 
#define JSONB_END_OF_READ_ALL -2 
#define JSONB_CONTINUE_READ 1
#define JSONB_READ_ERROR 0
#define JSONB_FLOAT_PRECISION "."
#define JSONB_OPENED_VALUE 0
#define JSONB_OPENED_VALUE_UNINITIATED -1
#define JSONB_OPENED_VALUE_CLOSED -2
#define JSONB_OPENED_VALUE_READ 1
#define JSONB_PARSER_TYPE_OBJECT 0
#define JSONB_PARSER_TYPE_ARRAY 1
#define JSONB_PARSER_TYPE_STR 2
#define JSONB_PARSER_TYPE_INT 3
#define JSONB_PARSER_TYPE_FLOAT 4
#define JSONB_PARSER_TYPE_NULL 5
#define JSONB_PARSER_TYPE_BOOLEAN 6
#define JSONB_PARSER_TYPE_UNKNOWN 7
#define JSONB_PARSER_TYPE_VALIDATED 8
#define JSONB_JAVA_STRING_SIGNATURE "Ljava/lang/String;"
#define JSONB_JAVA_INT_SIGNATURE "I"
#define JSONB_JAVA_FLOAT_SIGNATURE "F"
#define JSONB_JAVA_BOOLEAN_SIGNATURE "Z"
#define JSONB_VALUE_INT 1
#define JSONB_VALUE_FLOAT 2
#define JSONB_VALUE_STR 3
#define JSONB_VALUE_BOOLEAN 4
#define JSONB_VALUE_NULL 5
#define JSONB_VALUE_OBJECT 6
#define JSONB_VALUE_ARRAY 7
#define JSONB_ERROR_BUF_LEN 1024
#include <jni.h>
#include <float.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

struct value_str;
struct value_int;
struct value_float;
struct value_dynamic;
struct value_array;
struct value_object;
struct opened_token;

struct value_str {
	char* value;
};
struct value_int {
	int value;
};
struct value_float {
	float value;
};

struct value_dynamic {
	int type;
	int int_value;
	jboolean jboolean_value;
	float float_value;
	char* str_value;
	char* raw_value;
	char* key;
	jstring* jstring_value;
	struct value_array* array_value;
	struct value_object* object_value;
	struct value_dynamic* next;
};
	
struct value_array {
	struct value_dynamic* head;
	struct value_dynamic* next;
	struct value_dynamic* current;
 	int len;
};

struct value_object {
	struct value_array* values;
};
struct opened_token {
	int index;
	int status;
};

struct parser {
	struct opened_token opened_json;
	struct opened_token opened_key;
	struct opened_token opened_colon;
	struct opened_token opened_comma;
	struct opened_token opened_value;
	struct opened_token opened_quote;
	struct opened_token opened_array_value;
	struct opened_token opened_escape;
	int type_validated;
	int index;
	int type;
	int code;
	int status;
	const char* str;
    char* error;
	char* key;
	char* end_token;
    char* keyword;
	struct value_array* values;
	struct parser* parent;
	struct parser* root;
	jobjectArray* fields;
	jclass cls;
	JNIEnv* env;
	int obj_index;
};


struct writer {
	char* output;
	jobjectArray objects;
	jobjectArray fields;
	jclass cls;
	JNIEnv* env;
	struct writer* parent;
	struct writer* root;
	int status;
	int obj_index;
};
	
void jsonb_parse_from_str(struct parser** parser, const char* str);
void jsonb_debugf(const char* fmt, ...);
void jsonb_write(JNIEnv* env, jobjectArray fields, jobjectArray objs, jclass cls, struct writer* writer);
void jsonb_write_obj(JNIEnv* env, jobjectArray fields, jobjectArray objs, jclass cls, struct writer* writer);

void jsonb_writer_validate_null_str(struct writer* writer, char** output, jstring strfield);
void jsonb_writer_validate_null_int(struct writer* writer, char** output, jint intfield);
void jsonb_writer_validate_null_float(struct writer* writer, char** output, jfloat floatfield);
void jsonb_writer_validate_boolean(struct writer* writer, char** output, jboolean booleanfield);
void jsonb_init_parser(struct parser* parser, int index, int type);
void jsonb_init_writer(struct writer* writer);
void jsonb_evaluate_determined_type(struct parser* parser);
void jsonb_init_opened_token(struct opened_token* opened);
void jsonb_resolve_parser(struct parser* parser);
void jsonb_resolve_child_parser(struct parser* parent, int type, char* keyword);
void jsonb_resolve_parser_token(struct parser* parser);
void jsonb_resolve_parser_token_object(struct parser* parser);
void jsonb_resolve_parser_token_array(struct parser* parser);
void jsonb_resolve_parser_token_str(struct parser* parser);
void jsonb_resolve_parser_token_int(struct parser* parser);
void jsonb_resolve_reserved_keyword(struct parser* parser);
void jsonb_add_token(char** context, char* token);
void jsonb_write_null(char** context);
void jsonb_store_value(struct parser* parent, struct parser* child);
void jsonb_store_jboolean(char* raw, struct value_dynamic* value);
void jsonb_init_parser_values(struct value_array* values);
void jsonb_parse_value_end(struct parser* parser);
void jsonb_parse_whitespace(struct parser* parser, char* token);
void jsonb_parse_escape(struct parser* parser, char* token);
void jsonb_init_dynamic_value(struct value_dynamic* value);
int jsonb_verify_int(char* token);
int jsonb_verify_opened_json(struct parser* parser, char* token);
int jsonb_verify_reserved_token(struct parser* parser, char* token, int len_read);
void jsonb_resolve_writer(struct writer* writer);
void jsonb_resolve_writer_object(struct writer* writer);


int jsonb_check_type_or_unknown(struct parser* parser, char* token, char* expects, int type);
char* jsonb_get_token(struct parser* parser);
char* jsonb_get_token_index(struct parser* parser, int* index);
int jsonb_Verify_int(char* token);
int jsonb_parser_is_root(struct parser* parser);
struct parser* jsonb_parser_get_root(struct parser* parser);
int jsonb_parser_parent_end(struct parser* parser, char* token);
int jsonb_parent_end(struct parser* parser, char* token);

void jsonb_set_opened_status(struct parser* parser);
void jsonb_set_type_validated(struct parser* parser, int value);
void jsonb_set_code_continue(struct parser* parser);
void jsonb_set_code_error(struct parser* parser, char* details); 
void jsonb_set_code_end(struct parser* parser);
void jsonb_set_code_end_all(struct parser* parser);
void jsonb_set_value_from_child(struct parser* parser, struct parser* child);
void jsonb_parser_value_next(struct parser* parser, struct parser* child);
void jsonb_set_opened(struct parser* parser, struct opened_token* opened, int value);
void jsonb_debug_parser(struct parser* parser);
void jsonb_debug_parser_value(struct parser* parser, struct value_dynamic* value);
void jsonb_reset_value(struct parser* parser, char* token);
void jsonb_reset_parser(struct parser* parser);
void jsonb_cleanup_parser(struct parser* parser);
void jsonb_free_array(struct parser* parser, struct value_array* value);

void jsonb_parser_get_str(struct parser* parser, const char** str);
void jsonb_parser_determine_end(struct parser* parser);
void jsonb_debug_array(struct parser* parser, struct value_array* array);
void jsonb_debug_key_value(struct parser* parser, struct value_dynamic* value);
void jsonb_debug_value(struct parser* parser, struct value_dynamic* value);
jobject jsonb_create_java_object_from_class(JNIEnv* env, jclass cls);
int jsonb_token_cmp(char* token, const char* target);
int jsonb_token_end_cmp(char* token, const char* target);
int jsonb_token_int_cmp(struct parser* parser, char* token);
int jsonb_token_str_cmp(struct parser* parser, char* token);
int jsonb_validate_parent_end(struct parser* parser, char* token);
void jsonb_parser_error(struct parser* parser, char* details);
void jsonb_parser_error_index(struct parser* parser, char* token);
void jsonb_parser_str_escape(struct parser* parser, char* token);
void jsonb_parser_str_add_char(struct parser* parser, char* token);
void jsonb_comma(struct parser* parser, char* token);
int jsonb_comma_child(struct parser* parser, char* token);
void jsonb_copy_llist(struct parser* parser, struct value_dynamic** ptr1, struct value_object* obj);
void jsonb_copy_llist_value(struct value_dynamic* src, struct value_dynamic* dst);
void jsonb_parser_index_inc(struct parser* parser, int inc_val);
void jsonb_parser_index_dec(struct parser* parser, int dec_val);

jstring jsonb_generic_write_obj(JNIEnv* env, jobject target, jclass cls, jobjectArray fields);
jstring jsonb_generic_write(JNIEnv* env, jobjectArray target, jclass cls, jobjectArray fields);

#endif
