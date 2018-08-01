#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./jsonb.h"


int jsonb_verify_opened_json(struct parser* parser, char* token)
{
  	if ( parser->type_validated == 0 ) {
	    jsonb_set_code_continue(parser);
	    return JSONB_PARSER_TYPE_VALIDATED;
	}
	int type_array, type_object, type_str, type_int, verify_int;
	type_array = jsonb_check_type_or_unknown(parser,
		 token,
		 JSONB_ARRAY_START_TOKEN,
		 JSONB_PARSER_TYPE_ARRAY);
	type_object = jsonb_check_type_or_unknown(parser,
		 token,
		 JSONB_OBJECT_START_TOKEN,
		 JSONB_PARSER_TYPE_ARRAY);
	type_str = jsonb_check_type_or_unknown(parser,
		 token,
		 JSONB_QUOTE,
		 JSONB_PARSER_TYPE_STR);
	verify_int = jsonb_verify_int(token);
	if ( type_array == 0 ) {
	     jsonb_set_type_validated(parser, 0);
	     jsonb_set_code_continue( parser );
	     return JSONB_PARSER_TYPE_ARRAY;
	} else if ( type_object == 0 ) {
	     jsonb_set_type_validated(parser, 0);
	     jsonb_set_code_continue( parser );
	     return JSONB_PARSER_TYPE_OBJECT;
	} else if ( type_str == 0 ) {
	     jsonb_set_type_validated(parser, 0);
	     jsonb_set_code_continue( parser );
	     return JSONB_PARSER_TYPE_STR;
	} else if (verify_int == JSONB_CONTINUE_READ) {
	     jsonb_set_type_validated(parser, 0);
	     jsonb_set_code_continue( parser );
	     return JSONB_PARSER_TYPE_INT;
	}

 	printf("JSON must begin with %s or %s\r\n",
		JSONB_ARRAY_START_TOKEN,
		JSONB_OBJECT_START_TOKEN);
	jsonb_set_code_error( parser );
	return -1;
}


int jsonb_check_type_or_unknown(struct parser* parser, char* token, char* expects, int type)
{
 	if ( strcmp(token, expects) == 0 
	   && 
	     ( parser->type == type || parser->type == JSONB_PARSER_TYPE_UNKNOWN)
	 ) {
	     return 0;
	 }
 	return -1;
}

int jsonb_parser_is_root(struct parser* parser)
{
	if ( parser->root == NULL ) {
		return 0;
	}
	return -1;
}

struct parser* jsonb_parser_get_root(struct parser* parser)
{
	if ( parser->root == NULL ) {
		return parser;
	}
	return parser->root;
}

int jsonb_parser_parent_end(struct parser* parser, char* token)
{
	int root;
	struct parser* parent = parser->parent;
	if ( parser->root == NULL ){
		return -1;
	}
	if ( parent->type == JSONB_PARSER_TYPE_OBJECT && strcmp( token, JSONB_OBJECT_END_TOKEN ) == 0) 
	{
		return 0;
	}
	if ( parent->type == JSONB_PARSER_TYPE_ARRAY && strcmp( token, JSONB_ARRAY_END_TOKEN ) == 0 )
	{
		return 0;
	}

	return -1;

}


int jsonb_parser_first_token(struct parser* parser)
{
	int root;
	root = jsonb_parser_is_root(parser);
	if ( root == 0 || ( parser->index == parser->parent->index ) ) {
	    return 0;
	}
  	return -1;
}

int jsonb_verify_int(char* token)
{
	long lnum;
	int num;
	char* end;
	lnum = strtol( token, &end, 10 /** base10 **/);
	if ( end == token ) {
	   return JSONB_READ_ERROR;
	
	}
	return JSONB_CONTINUE_READ;

}

int jsonb_verify_str(char* token)
{
    int token_int = (int) *token;
    if( isalpha( token_int ) ) {
         return JSONB_CONTINUE_READ;
    }
    return JSONB_READ_ERROR;
}

int jsonb_reserved_tokens(struct parser* parser)
{
    int status;
    status = jsonb_verify_reserved_token( parser, JSONB_NULL, 4 );

    if ( status == JSONB_CONTINUE_READ ) {
      jsonb_resolve_child_parser(parser, JSONB_PARSER_TYPE_NULL);
      return status;
   }
   status = jsonb_verify_reserved_token( parser, JSONB_TRUE, 4 );
   if ( status == JSONB_CONTINUE_READ ) {
     jsonb_resolve_child_parser(parser, JSONB_PARSER_TYPE_BOOLEAN);
     return status;
   }
   status = jsonb_verify_reserved_token( parser, JSONB_FALSE, 5 );
   if ( status == JSONB_CONTINUE_READ ) {
     return status;
   }
   return JSONB_READ_ERROR;
}

int jsonb_verify_null(struct parser* parser)
{
	char* val = "";
	printf("jsonb_verify_null\r\n");
	int idx;
	idx = parser->index;
	jsonb_add_token(&val, jsonb_get_token_index(parser, &idx));
	idx += 1;
	jsonb_add_token(&val, jsonb_get_token_index(parser, &idx));
	idx += 1;
	jsonb_add_token(&val, jsonb_get_token_index(parser, &idx));
	idx += 1;
	jsonb_add_token(&val, jsonb_get_token_index(parser, &idx));
	if ( strcmp( val, JSONB_NULL ) == 0 ) {
		return JSONB_CONTINUE_READ;
	}
	return JSONB_READ_ERROR;
}

int jsonb_verify_reserved_token(struct parser* parser, char* token, int len_read)
{
	jsonb_printf("jsonb_verify_reserved_token token %s\r\n", token);
	char* val = "";
    char* token_idx;
	int idx;
    idx = parser->index;
    while ( ( idx - parser->index ) != len_read ) {
        token_idx = jsonb_get_token_index(parser, &idx);
        jsonb_add_token(&val, token_idx);
        ++ idx;
    }
    jsonb_printf("jsonb_verify_reserved_token result %s\r\n", val);
	if ( strcmp( val, token ) == 0 ) {
		return JSONB_CONTINUE_READ;
	}
	return JSONB_READ_ERROR;
}

void jsonb_debug_parser(struct parser* parser)
{
	const char* str;
	jsonb_parser_get_str( parser, &str );
	printf("PARSER: OPENED_JSON %d\r\n", parser->opened_json.status);
	printf("PARSER: OPENED_KEY %d\r\n", parser->opened_key.status);
 	printf("PARSER: OPENED_COLON %d\r\n", parser->opened_colon.status);
 	printf("PARSER: OPENED_COMMA %d\r\n", parser->opened_comma.status);
	printf("PARSER: KEY %s\r\n", parser->key);
	printf("PARSER: RAW VALUE %s\r\n", parser->values->current->raw_value);
	printf("PARSER: CODE %d\r\n", parser->code);
	printf("PARSER: STATUS %d\r\n", parser->status);
	printf("PARSER: INDEX %d\r\n", parser->index);
	printf("PARSER: TYPE VALIDATED %s\r\n", parser->type_validated);
	printf("PARSER: TYPE %d\r\n", parser->type);
	printf("PARSER: STR %s\r\n", str);
}
void jsonb_debug_parser_value(struct parser* parser, struct value_dynamic* value)
{
	char* key;
	char* value_str;
	if ( jsonb_parser_is_root( parser ) != 0 && parser->parent->type == JSONB_PARSER_TYPE_OBJECT ) {
	 	key = parser->parent->key;
	}
	printf("PARSER VALUE: KEY %s\r\n", key);
	printf("PARSER VALUE: %s\r\n", value_str );
}

void jsonb_debug_array(struct parser* parser, struct value_array*  array)
{
	printf("jsonb_debug_array\r\n");
	struct value_dynamic* ptr;
	ptr = array->head;
	printf("jsonb_debug_array head value %s\r\n", array->current->raw_value);
	while ( ptr != NULL ) {
		jsonb_debug_value( parser, ptr );
		ptr = ptr->next;
	}
	ptr = array->head;
}

void jsonb_free_array(struct parser* parser, struct value_array*  array)
{
	printf("jsonb_free_array\r\n");
	struct value_dynamic** ptr;
	ptr = &array->head;
	printf("jsonb_debug_array head value %s\r\n", array->current->raw_value);
	while ( *ptr != NULL ) {
		free( *ptr );
		ptr = &((*ptr)->next);
	}
	free( parser->values );
}


void jsonb_debug_value(struct parser* parser, struct value_dynamic* value)
{
	printf("jsonb_debug_value\r\n");
	printf("key: %s value: %s\r\n", value->key, value->raw_value);
}
void jsonb_debug_key_value(struct parser* parser, struct value_dynamic* value)
{
	printf("jsonb_debug_key_value\r\n");
	printf("key: %s, value: %s\r\n", parser->key, value->raw_value);
}


void jsonb_dynamic_value_as_char(struct value_dynamic* value)
{
}

void jsonb_set_code_continue(struct parser* parser)
{
	parser->code = JSONB_CONTINUE_READ;
}

void jsonb_set_code_error(struct parser* parser)
{
	parser->code = JSONB_READ_ERROR;
}

void jsonb_set_code_end(struct parser* parser)
{
	parser->code = JSONB_END_OF_READ;
}

void jsonb_set_code_end_all(struct parser* parser)
{
	parser->code = JSONB_END_OF_READ_ALL;
}


void jsonb_set_value_from_child(struct parser* parser, struct parser* child)
{
	int index = child->index;
	parser->index = child->index;
	char* set_value;
	jsonb_parser_value_next( parser, child );
	if ( child->type == JSONB_PARSER_TYPE_ARRAY ) {
		jsonb_debug_array(parser, child->values);
	} else {
		jsonb_debug_value(parser, child->values->current);
	}
	jsonb_reset_parser(parser);

}

void jsonb_set_opened_status(struct parser* parser)
{
	parser->opened_json.status = JSONB_OPENED_VALUE;
	parser->index += 1;
}

void jsonb_set_type_validated(struct parser* parser, int value)
{
	parser->type_validated = value;
}

void jsonb_set_opened(struct parser* parser, struct opened_token* opened_token, int opened)
{
	if ( opened == JSONB_OPENED_VALUE ) {
		opened_token->index = parser->index;
		opened_token->status = opened;
		return;
	}
	opened_token->index = -1;
	opened_token->status = opened;
}


void jsonb_init_opened_token(struct opened_token* opened)
{
	// index is when parser finds this token
	opened->status = -1;
	opened->index = -1;
}

void jsonb_cleanup_parser(struct parser* parser)
{
	printf("jsonb_cleanup_parser \r\n");
	jsonb_free_array( parser, parser->values );
	free( parser );
}

void jsonb_reset_parser(struct parser* parser)
{
	jsonb_set_opened(parser, &parser->opened_value, -1);
	jsonb_set_opened(parser, &parser->opened_key, -1);
	jsonb_set_code_continue( parser );
}



void jsonb_init_parser(struct parser* parser, int index, int type)
{
	jsonb_init_opened_token(&parser->opened_key);
	jsonb_init_opened_token(&parser->opened_colon);
	jsonb_init_opened_token(&parser->opened_comma);
	jsonb_init_opened_token(&parser->opened_value);
	jsonb_init_opened_token(&parser->opened_quote);
	jsonb_init_opened_token(&parser->opened_escape);
	parser->key = '\0';
  	parser->index = index;
	parser->code = JSONB_CONTINUE_READ;
	parser->status = 0;
	parser->type = type;
	parser->type_validated = -1;
	parser->values = (struct value_array*) malloc(sizeof(struct value_array));
	parser->values->current = (struct value_dynamic*) malloc(sizeof(struct value_dynamic));
	parser->values->current->raw_value = "";
	parser->values->current->next = NULL;
	parser->values->current->key = NULL;
	parser->values->len = 1;
	parser->values->head = parser->values->current;

	jsonb_evaluate_determined_type( parser );
}


void jsonb_init_writer(struct writer* writer)
{
	writer->output = "";
	writer->obj_index = 0;
}

void jsonb_evaluate_determined_type( struct parser* parser )
{
	char* token;

	token = jsonb_get_token( parser );
	int evaluated = jsonb_verify_opened_json( parser, token );
	if ( evaluated == JSONB_PARSER_TYPE_VALIDATED ) {
		return;
	} else if ( evaluated == -1 ) { // error 
		jsonb_set_code_error(parser);
		return;
	}
	parser->type = evaluated;
}

void jsonb_init_dynamic_value( struct value_dynamic* ptr )
{
	ptr->raw_value = "";
}
void jsonb_init_parser_values( struct value_array* ptr )
{
	jsonb_init_dynamic_value( ptr->current );
	ptr->head = ptr->current;
}
void jsonb_parser_value_next( struct parser* parser, struct parser* child)
{
	// add a new value to linked list
	//
	struct value_dynamic* new_value;
	// set the key on the value
	jsonb_store_value(parser, child);
	// cast the current value to java types TODO
	if ( child->opened_comma.status == 0 ) {	
		new_value = (struct value_dynamic*) malloc(sizeof(struct value_dynamic));
		new_value->raw_value = "";
		new_value->next = NULL;
		new_value->key = NULL;

		parser->values->current->next = new_value;
		parser->values->current = new_value;
		parser->values->len += 1;
	}

}
void jsonb_add_token(char** context, char* token)
{
	char* new_context;
	int len = strlen( *context );
	int token_len = strlen( token );
	int total_len = ( len + token_len );
	new_context = (char*) malloc( (total_len) + 1 );
	strcpy( new_context, *context );
	strcat( new_context, token );
	new_context[ total_len ] = '\0';
	*context = new_context;
}
void jsonb_write_null(char** context)
{
	jsonb_add_token(context, "null");
}
void jsonb_store_value( struct parser* parent, struct parser* child)
{
	printf("jsonb_store_value\r\n");
	struct parser* root = jsonb_parser_get_root(parent);
	struct value_dynamic* current = child->values->current;
	char* value_str;
	// store the key of the current item
	parent->values->current->key = strdup( parent->key );
	printf("parent key: %s, type: %d, child parser value is %s, type: %d\r\n", parent->key, parent->type, current->raw_value, child->type);
	parent->values->current->type = child->type;
	if ( child->type == JSONB_PARSER_TYPE_STR ) {
		printf("casting to string\r\n");
		const char* val = (const char*) current->raw_value;
		parent->values->current->str_value = strdup( current->raw_value );
	} else if ( child->type == JSONB_PARSER_TYPE_INT ) {
		printf("casting to int\r\n");
		int val = atoi( current->raw_value );
		parent->values->current->int_value = val;
	} else if ( child->type == JSONB_PARSER_TYPE_FLOAT ) {
		printf("casting to float\r\n");
		float val = atof( current->raw_value );
		parent->values->current->float_value = val;
	} else if ( child->type == JSONB_PARSER_TYPE_ARRAY ) {
		printf("Cast found array no casting needed\r\n");
		parent->values->current->array_value = child->values;
	} else if ( child->type == JSONB_PARSER_TYPE_OBJECT ) {
		printf("Cast found object no casting needed\r\n");
		parent->values->current->object_value = (struct  value_object*) malloc(sizeof( struct value_object ) );
		parent->values->current->object_value->values = child->values;
	} else if ( child->type == JSONB_PARSER_TYPE_NULL ) {
		printf("Cast found null\r\n");
	} else if ( child->type == JSONB_PARSER_TYPE_BOOLEAN ) {
		printf("Cast found boolean\r\n");
        jsonb_store_jboolean( current->raw_value, parent->values->current );
    }

	printf("Setting raw value to child\r\n");
	//parent->values->current->raw_value = strdup( current->raw_value );
	free( current->raw_value );
}

void jsonb_store_jboolean(char* raw, struct value_dynamic* value)
{
    if ( strcmp( raw, JSONB_TRUE ) == 0 ) {
       value->jboolean_value = 1;
    } else if ( strcmp( raw, JSONB_FALSE ) == 0 ) {
       value->jboolean_value = 0;
    }
               
}
void jsonb_parser_get_str(struct parser* parser, const char** str)
{
	int root;
	root = jsonb_parser_is_root( parser );
	if ( root != 0 ) {
		*str = parser->root->str;
	} else {
		*str = parser->str;
	}
	
}

char* jsonb_get_token_index(struct parser* parser, int* index)
{
	char my_char;
       	const char* str;
	char* token;
	jsonb_parser_get_str( parser, &str );
    size_t len = strlen( str );
    if ((*index) < len) {
        my_char = str[ *index ];
        token = (char*)malloc(sizeof(char));
        *token = my_char;
        token[1] = '\0';
        return token;
    }
    token = NULL;
    return token;
}

char* jsonb_get_token(struct parser* parser)
{
	return jsonb_get_token_index(parser, &parser->index);
}

void jsonb_resolve_parser( struct parser* parser)
{
	printf("jsonb_resolve_parser type %d\r\n", parser->type);
	const char* str;
	jsonb_parser_get_str( parser, &str );
	int size = strlen( str );
	char* value;
	int initial;
	int status;
 	initial = parser->index;
	while ( parser->index < size ) {
		char* token = jsonb_get_token(parser);
		jsonb_resolve_parser_token(parser);
		if ( parser->code == JSONB_READ_ERROR ) {
			parser->status = -1;
			return;
		}

		if ( parser->code == JSONB_END_OF_READ ) {
			parser->status = 0;
			printf("reached end of read. status is %d\r\n", parser->status);
			break;
		}
		parser->index += parser->code;
	} 
}

void jsonb_resolve_child_parser(struct parser* parent, int type)
{
	printf("jsonb_resolve_child_parser type %d index %d\r\n", type, parent->index);
	int status, parent_is_root;
	size_t sz = sizeof(struct parser);
	struct parser* parser = (struct parser*) malloc(sz* ( 1024 * 1024 ));
	parser->parent = parent;
	parser->root = parent;
	parent_is_root = jsonb_parser_is_root( parent );
	if ( parent_is_root != 0 ) {
		parser->root = parent->root;
	}
	jsonb_init_opened_token(&parser->opened_key);
	jsonb_init_opened_token(&parser->opened_colon);
	jsonb_init_opened_token(&parser->opened_comma);
	jsonb_init_opened_token(&parser->opened_value);
	jsonb_init_opened_token(&parser->opened_quote);
	jsonb_init_opened_token(&parser->opened_escape);

	parser->values = (struct value_array*) malloc(sizeof(struct value_array));
	parser->values->current = (struct value_dynamic*) malloc(sizeof(struct value_dynamic));
	parser->values->current->raw_value = "";
	parser->values->current->next = NULL;
	parser->values->current->key = NULL;
	parser->values->len = 1;
	parser->values->head = parser->values->current;
	parser->key = '\0';
	parser->type = type;
	parser->type_validated = 0;
	parser->code = JSONB_CONTINUE_READ;
	parser->index = parent->index;
	//parser->values->current->raw_value = strdup(parent->values->current->raw_value);
	//parser->key = strdup(parent->key);
	jsonb_resolve_parser(parser);
	if ( parser->code != JSONB_END_OF_READ ) {
		jsonb_set_code_error( parent );
	}
	jsonb_set_code_continue( parent );
	jsonb_set_value_from_child(parent, parser);
	jsonb_cleanup_parser(parser);

}

void jsonb_resolve_parser_token(struct parser* parser)
{
	if ( parser->type == JSONB_PARSER_TYPE_ARRAY ) {
		jsonb_resolve_parser_token_array( parser );
	} else if ( parser->type == JSONB_PARSER_TYPE_OBJECT ) {
	 	jsonb_resolve_parser_token_object( parser );
	} else if ( parser->type == JSONB_PARSER_TYPE_STR ) {
	 	jsonb_resolve_parser_token_str( parser );
	} else if ( parser->type == JSONB_PARSER_TYPE_NULL ) {
	 	jsonb_resolve_reserved_keyword( parser );
    } else if ( parser->type == JSONB_PARSER_TYPE_BOOLEAN ) {
	 	jsonb_resolve_reserved_keyword( parser );
	} else if ( 
	   parser->type == JSONB_PARSER_TYPE_INT
	   ||
	   parser->type == JSONB_PARSER_TYPE_FLOAT
	   ) {
	 	jsonb_resolve_parser_token_int( parser );
	}
}

void jsonb_resolve_writer(struct writer* writer)
{
	char** contents;
	int* idx;
	jsize len;
	JNIEnv* env;
	env = writer->env;
	contents = &(writer->output);
	jsonb_add_token(contents, "[");
	idx = &(writer->obj_index);
	len = (*env)->GetArrayLength(env, writer->objects);
	while ( *idx < len ) {
		jsonb_resolve_writer_object(writer);
		if ( len > 1 && *idx < (len-1) ) {
			jsonb_add_token(contents, JSONB_COMMA);
		}
		++ *idx;
	}
	jsonb_add_token(contents, "]");
}

void jsonb_resolve_writer_object(struct writer* writer)
{
	jsize field_len;
	int i;
	char** output = &writer->output;
	jclass cls = writer->cls;
	JNIEnv* env = writer->env; 
	jsize index = writer->obj_index;
	jobject obj = (*env)->GetObjectArrayElement(env, writer->objects, index);
	field_len = (*env)->GetArrayLength(env, writer->fields);
	jclass field_cls= (*env)->FindClass(env, "com/jsonb/ExecJar/ReflectionType");
	jsonb_add_token( output, "{" );
	jstring strfield;
	jfieldID val_field;
	const char* key;
	const char* type_c;
	char* type;
	jobjectArray field_arr;

	for ( i = 0; i < field_len; i ++ ) {
		field_arr = (jobjectArray) (*env)->GetObjectArrayElement(env, writer->fields, i);
		strfield = (jstring) (*env)->GetObjectArrayElement(env, field_arr, 0);
		type_c= (*env)->GetStringUTFChars(env, strfield, 0);
		key = strdup(type_c);
		(*env)->ReleaseStringUTFChars(env, strfield, type_c);
		strfield = (jstring) (*env)->GetObjectArrayElement(env, field_arr, 1);
		type_c = (char*) (*env)->GetStringUTFChars(env, strfield, 0);
		(*env)->ReleaseStringUTFChars(env, strfield, type_c);
		jsonb_add_token( output, "\"" );
		jsonb_add_token( output, (char*) key );
		jsonb_add_token( output, "\"" );
		jsonb_add_token( output, ":" );
		printf("found type %s\r\n", type_c);

		if ( strcmp( type_c, JSONB_JAVA_STRING_SIGNATURE ) == 0 ) {
			printf("jsonb_resolve_writer_obj writing string\r\n");
			val_field = (*env)->GetFieldID(env, cls, key, "Ljava/lang/String;");
			strfield = (jstring) (*env)->GetObjectField(env, obj, val_field);
			jsonb_writer_validate_null_str( writer, output, strfield );
		} else if ( strcmp( type_c, JSONB_JAVA_INT_SIGNATURE ) == 0 ) { 
			printf("jsonb_resolve_writer_obj writing int\r\n");
			val_field = (*env)->GetFieldID(env, cls, key, "I");
			char* int_chr;
			jint val = (*env)->GetIntField(env, obj, val_field);
			jsonb_writer_validate_null_int( writer, output, val );
		} else if ( strcmp( type_c, JSONB_JAVA_FLOAT_SIGNATURE ) == 0 ) {
			printf("jsonb_resolve_writer_obj writing float\r\n");
			val_field = (*env)->GetFieldID(env, cls, key, "F");
			char* float_chr;
			jfloat val = (*env)->GetFloatField(env, obj, val_field);
			jsonb_writer_validate_null_float( writer, output, val );
        } else if ( strcmp( type_c, JSONB_JAVA_BOOLEAN_SIGNATURE ) == 0 ) {
			printf("jsonb_resolve_writer_obj writing boolean\r\n");
			val_field = (*env)->GetFieldID(env, cls, key, "Z");
			jboolean val;
			val = (*env)->GetBooleanField(env, obj, val_field);
			jsonb_writer_validate_boolean( writer, output, val );

		} else { // type not supported
			printf("jsonb_resolve_writer_obj type not supported\r\n");
			jsonb_write_null(output);
		}
				
		if ( (field_len-1) != i ) {
			jsonb_add_token( output, "," );
		}
	}
	jsonb_add_token(output, "}");
}



			

			
			
		 	
		
			

		

	


void jsonb_resolve_parser_token_array(struct parser* parser)
{
	char* token = jsonb_get_token(parser);
	printf("jsonb_resolve_parser_token_array vtoken %s\r\n", token);
	int parent_end = jsonb_parser_parent_end(parser, token);
 	if ( jsonb_token_cmp( token, JSONB_QUOTE ) == 0 ) { // has to be string
		printf("Parsing str value\r\n");
		jsonb_resolve_child_parser(
				parser,
				JSONB_PARSER_TYPE_STR);
		return;
	} else if ( jsonb_token_int_cmp( parser, token ) == 0 ) {
		jsonb_resolve_child_parser(
		parser,
		JSONB_PARSER_TYPE_INT);
		return;
	} else if ( jsonb_token_cmp( token, JSONB_WHITESPACE )  == 0 ) {
		jsonb_set_code_continue( parser );
		return;
	} else if ( jsonb_token_cmp( token, JSONB_ESCAPE ) == 0 ) {
        jsonb_parser_error_index( parser, token );
		return;
	} else if ( jsonb_token_cmp( token, JSONB_COLON ) == 0 )  {
        jsonb_parser_error_index( parser, token );
		return;

	} else if ( jsonb_token_cmp( token, JSONB_ARRAY_START_TOKEN ) == 0 ) {
		jsonb_set_code_continue( parser );
		return;
	} else if ( jsonb_token_end_cmp( token, JSONB_ARRAY_END_TOKEN ) == 0) {
		jsonb_reset_parser( parser );
		jsonb_set_code_end( parser );
		return;
	} else if ( jsonb_token_cmp( token, JSONB_OBJECT_START_TOKEN ) == 0 ) {
		parser->obj_index += 1;

        jsonb_resolve_child_parser(
        parser,
        JSONB_PARSER_TYPE_OBJECT);
		return;
	} else if ( jsonb_token_cmp( token, JSONB_OBJECT_END_TOKEN ) == 0 ) {
		jsonb_set_code_continue( parser );
		return;
	} else if ( jsonb_token_cmp( token, JSONB_COMMA ) == 0 ) {
		//new value
		jsonb_reset_parser( parser );
		jsonb_set_code_continue( parser );
		return;
	} 

    if ( jsonb_reserved_tokens( parser ) == JSONB_CONTINUE_READ ) {
       return;
     }
    if ( jsonb_validate_parent_end( parser, token ) == 0 ) {
       return;
    }
    jsonb_parser_error_index(  parser, token );
}

// parsing for objects
void jsonb_resolve_parser_token_object(struct parser* parser)
{
	char* token = jsonb_get_token(parser);
	printf("jsonb_resolve_parser_token_object token: %s, index: %d\r\n", token, parser->index);

	if ( jsonb_token_cmp( token, JSONB_QUOTE ) == 0 ) {
		 if ( parser->opened_key.status == -1 ) {
			jsonb_set_opened(parser, &parser->opened_key, JSONB_OPENED_VALUE);
			parser->key = "";
			jsonb_set_code_continue( parser );
			return;
		 }
		 if ( parser->opened_key.status  == 0 ){ // ended key
			jsonb_set_opened(parser, &parser->opened_key, JSONB_OPENED_VALUE_READ);
			jsonb_set_code_continue( parser );
			return;
		 }
		 if (parser->opened_value.status == 0) { // new str value
			printf("Parsing str value\r\n");
			jsonb_resolve_child_parser(
					parser,
					JSONB_PARSER_TYPE_STR);
			return;
		 }
         jsonb_parser_error_index( parser, token );
	} else if ( jsonb_token_cmp( token, JSONB_COLON ) == 0 ) {
		jsonb_set_opened(parser, &parser->opened_value, JSONB_OPENED_VALUE);
		jsonb_set_code_continue( parser );
		return;
	} else if ( jsonb_token_cmp( token, JSONB_WHITESPACE ) == 0 ) {
		 if ( parser->opened_key.status == 0 ) {
			jsonb_add_token(&parser->key, token);
			jsonb_set_code_continue( parser );
			return;
		 }
		 return;
	} else if ( jsonb_token_cmp( token, JSONB_ARRAY_START_TOKEN ) == 0 ) {
		if ( parser->opened_value.status == 0 ) {
		    jsonb_resolve_child_parser(
			 	parser,
				JSONB_PARSER_TYPE_ARRAY);
		    return;
		}
	} else if ( jsonb_token_cmp( token, JSONB_ESCAPE ) == 0 ) {
		 jsonb_add_token(&parser->values->current->raw_value, token);
		 jsonb_set_code_continue( parser );
		 return;
 	} else if ( jsonb_token_cmp( token, JSONB_COMMA ) == 0 ) {
		 jsonb_reset_parser(parser);
	} else if ( jsonb_token_cmp( token, JSONB_OBJECT_START_TOKEN ) == 0 ) {
		if ( parser->opened_json.status != 0 ) {
			jsonb_set_opened(parser, &parser->opened_json, JSONB_OPENED_VALUE);
			parser->obj_index = 0;
			return;
		}
		if ( parser->opened_value.status != 0 ) {
			jsonb_set_code_continue( parser );
			return;
		}
		if ( parser->opened_value.status == 0 ) {
	 		printf("Nested objects not implemented\r\n");
			jsonb_set_code_error( parser );
		}
        jsonb_parser_error_index( parser, token );
		return;
    } else if ( jsonb_token_int_cmp( parser, token ) == 0 ) {
        jsonb_resolve_child_parser(
        parser,
        JSONB_PARSER_TYPE_INT);
        return;
	} else if ( jsonb_token_end_cmp( token, JSONB_OBJECT_END_TOKEN ) == 0 ) {
		if ( jsonb_parser_is_root( parser ) != 0 ){
			jsonb_set_code_end( parser );
			return;
		}
		jsonb_reset_parser( parser );
		return;
    }
    jsonb_printf("jsonb_resolve_parser_token_obj adding to key\r\n");

    // any other value
    if ( parser->opened_key.status == 0 ) {
      jsonb_add_token(&parser->key, token );
      jsonb_set_code_continue( parser );
      return;
    }
    if ( jsonb_reserved_tokens( parser ) == JSONB_CONTINUE_READ ) {
       return;
     }
    jsonb_parser_error_index(  parser, token );
}

void jsonb_resolve_parser_token_str(struct parser* parser)
{
	char* token = jsonb_get_token(parser);
	printf("jsonb_resolve_parser_token_str\r\n");
	if ( strcmp(token, JSONB_QUOTE ) == 0 ) { 
		if ( parser->opened_value.status != 0 ) {
			jsonb_set_opened(parser, &parser->opened_value, JSONB_OPENED_VALUE);
			jsonb_set_code_continue(parser);
			return;
		}
		if ( parser->opened_value.status == 0 ) {
			jsonb_set_opened(parser, &parser->opened_value, -1);
			jsonb_set_code_continue(parser);
			return;
		}
		if ( parser->opened_escape.status == 0 ) {
			jsonb_add_token( &parser->values->current->raw_value, token );
			jsonb_set_code_continue( parser );
			return;
		}
		printf("parsed string value %s\r\n", parser->values->current->raw_value);
		jsonb_set_code_end( parser );
		return;	
	} else if ( strcmp(token, JSONB_COMMA) == 0 ) {
		if ( parser->opened_value.status != 0 ) {
			jsonb_set_opened(parser, &parser->opened_comma, JSONB_OPENED_VALUE);
			jsonb_set_code_end( parser );
			return;
		}
		jsonb_add_token( &parser->values->current->raw_value, token );
		jsonb_set_code_continue( parser );
		return;

	} else if ( strcmp(token, JSONB_ESCAPE) == 0 ) {
		if ( parser->opened_escape.status == 0 ) {
			jsonb_add_token( &parser->values->current->raw_value, token );
			jsonb_set_code_continue( parser );
			return;
		}
		jsonb_set_opened(parser, &parser->opened_escape, JSONB_OPENED_VALUE);
		return;
	} else if (jsonb_token_str_cmp( parser, token ) == 0 ) {
        jsonb_add_token(&parser->values->current->raw_value, token);
		jsonb_set_code_continue( parser );
        return;
    }

    if ( jsonb_validate_parent_end( parser, token ) == 0 ) {
		jsonb_add_token(&parser->values->current->raw_value, token);
		jsonb_set_code_continue( parser );
		return;
	}
    jsonb_parser_error_index(parser, token);
}

void jsonb_resolve_reserved_keyword(struct parser* parser)
{
	jsonb_printf("jsonb_resolve_reserved_keyword index %d\r\n", parser->index);

	char* token = jsonb_get_token(parser);
	int len = strlen( parser->values->current->raw_value );
	int parent_end = jsonb_parser_parent_end(parser, token);
	if ( strcmp( token, JSONB_COMMA ) == 0 ) {
		jsonb_set_opened( parser, &parser->opened_comma, JSONB_OPENED_VALUE );
		jsonb_set_code_end( parser );
		return;
	} else if ( parent_end == 0 ) {
		jsonb_set_code_end( parser );
		return;
	} else { 
		jsonb_add_token( &parser->values->current->raw_value, token );
		jsonb_set_code_continue( parser );
    }

}

void jsonb_resolve_parser_token_null(struct parser* parser)
{
	printf("jsonb_resolve_parser_token_null index %d\r\n", parser->index);

	char* token = jsonb_get_token(parser);
	int len = strlen( parser->values->current->raw_value );
	int parent_end = jsonb_parser_parent_end(parser, token);

	if ( strcmp( token, "n" ) == 0 ) {
	 	jsonb_set_opened( parser, &parser->opened_value, JSONB_OPENED_VALUE );
		jsonb_add_token( &parser->values->current->raw_value, token );
		jsonb_set_code_continue( parser );
	} else if ( strcmp( token, "u" ) == 0 && len == 1 ) {
		jsonb_add_token( &parser->values->current->raw_value, token );
		jsonb_set_code_continue( parser );
	} else if ( strcmp( token, "l" ) == 0 && ( len == 2 ) )  {
		jsonb_add_token( &parser->values->current->raw_value, token );
		jsonb_set_code_continue( parser );
	} else if ( strcmp( token, "l" ) == 0 && ( len == 3 ) )  {
		jsonb_add_token( &parser->values->current->raw_value, token );
		jsonb_set_opened( parser, &parser->opened_value, -1 );
		jsonb_set_code_continue( parser );
	} else if ( strcmp( token, JSONB_COMMA ) == 0 ) {
		jsonb_set_opened( parser, &parser->opened_comma, JSONB_OPENED_VALUE );
		jsonb_set_code_end( parser );
		return;
	} else if ( parent_end == 0 ) {
		jsonb_set_code_end( parser );
		return;
	}

}


void jsonb_resolve_parser_token_int(struct parser* parser)
{
	printf("jsonb_resolve_parser_token_int %d\r\n", parser->index);
	char* token = jsonb_get_token(parser);
	int verify = jsonb_verify_int( token );
	int parent_end = jsonb_parser_parent_end(parser, token);
	if ( verify == JSONB_CONTINUE_READ ) {
		printf("jsonb_resolve_parser_token_int value %s\r\n", token);	

		jsonb_add_token(&parser->values->current->raw_value, token );
		jsonb_set_code_continue( parser );
		return;
	} else if ( strcmp( token, JSONB_FLOAT_PRECISION ) == 0 ) {
		parser->type = JSONB_PARSER_TYPE_FLOAT;
		jsonb_add_token(&parser->values->current->raw_value, token );
		jsonb_set_code_continue( parser );
		return;
	} else if ( strcmp( token, JSONB_COMMA ) == 0 ) {
		jsonb_set_code_end( parser );
		jsonb_set_opened( parser, &parser->opened_comma, JSONB_OPENED_VALUE );
		return;
	} else if ( strcmp( token, JSONB_WHITESPACE) == 0 ) {
		jsonb_set_code_end( parser );
		return;
	} else if ( parent_end == 0 ) {
		parser->index -= 1;
		jsonb_set_code_end( parser );

		return;
	} else {
		printf("illegal value for int/float '%s'\r\n", token);
		jsonb_set_code_error( parser );
		return;
	}


}

int jsonb_parse_from_str(struct parser** target, const char* str)
{
	printf("Parsing JSONB string: %s\r\n", str);
	struct parser* parser = (struct parser*) malloc(sizeof(struct parser));
	parser->str = str;
	parser->root = NULL;
	parser->parent = NULL;
	jsonb_init_parser( 
		parser, 
		0 /** index **/, 
		JSONB_PARSER_TYPE_UNKNOWN);
	jsonb_resolve_parser( parser );
	printf("parser status is %d\r\n", parser->status);
	if (parser->status != 0) {
		jsonb_cleanup_parser( parser );
		return -1;
	}
	if (jsonb_parser_is_root(parser)!=0){
		jsonb_cleanup_parser( parser );
	}
	*target = parser;
	return 0;
}

void jsonb_write(JNIEnv* env, jobjectArray fields, jobjectArray objs, jclass cls, struct writer* writer)
{
	writer->env = env;
	writer->objects = objs;
	writer->fields = fields;
	writer->cls = cls;
	jsonb_init_writer(writer);
	jsonb_resolve_writer(writer);
}

void jsonb_writer_validate_null_str(struct writer* writer, char** output, jstring strfield)
{
	if ( strfield == NULL ) {
		jsonb_write_null( output );
		return;
	}
	JNIEnv* env = writer->env;
	char* type_c = (char*) (*env)->GetStringUTFChars(env, strfield, 0);
	jsonb_add_token( output, "\"" );
	jsonb_add_token( output, (char*) type_c );
	jsonb_add_token( output, "\"" );
	(*env)->ReleaseStringUTFChars(env,strfield,type_c);
}

void jsonb_writer_validate_null_int(struct writer* writer, char** output, jint intfield)
{
	char* int_chr;
	JNIEnv* env = writer->env;
	sprintf(int_chr, "%d", intfield);
	jsonb_add_token( output, int_chr );

}

void jsonb_writer_validate_null_float(struct writer* writer, char** output, jfloat floatfield)
{
	char* float_chr;
	JNIEnv* env = writer->env;
	sprintf(float_chr, "%f", floatfield);
	jsonb_add_token( output, float_chr );
}

void jsonb_writer_validate_boolean(struct writer* writer, char** output, jboolean booleanfield)
{
    jsonb_printf("jsonb_writer_validate_boolean value for boolean is %d\r\n", booleanfield);
    if (booleanfield == 1) {
        jsonb_add_token( output, JSONB_TRUE );
    } else if ( booleanfield == 0 ) {
        jsonb_add_token( output, JSONB_FALSE );
    }
}

void jsonb_write_obj(JNIEnv* env, jobjectArray fields, jobjectArray objs, jclass cls, struct writer* writer)

{
	writer->env = env;
	writer->objects = objs;
	writer->fields = fields;
	writer->cls = cls;
	jsonb_init_writer(writer);
	jsonb_resolve_writer_object(writer);

}

jobject jsonb_create_java_object_from_class(JNIEnv* env, jclass target)
{
   jmethodID constructor;
   jobject object;
   constructor = (*env)->GetMethodID(env, target, "<init>", "()V");
   object = (*env)->NewObject(env, target, constructor);
   return object;
}

jobjectArray jsonb_reset_java_array(JNIEnv* env, jclass target, jobjectArray* current)
{
	jint i;
	jsize currlen = (*env)->GetArrayLength(env, *current);
	jsize newlen = (currlen+1);
	jobject obj = jsonb_create_java_object_from_class( env, target );	
	jobjectArray new_array = (*env)->NewObjectArray(env, newlen, target, obj);
	printf("JNI array currlen %d, newlen %d\r\n", currlen, newlen);
	for ( i = 0; i < currlen; i ++ ) {
		jobject obj = (*env)->GetObjectArrayElement(env, target, i);
		(*env)->SetObjectArrayElement(env, new_array, i, obj);
	}
	printf("created a new array based on new length\r\n");
	return new_array;

}

jobject jsonb_writer_get_current_obj(struct writer* writer)
{
	struct writer* root;
	jobject obj;
	JNIEnv* env;
	env = writer->env;
	jsize idx = (jsize) writer->obj_index;
	obj = (*env)->GetObjectArrayElement(env, writer->objects, idx); 
	return obj;
}

void jsonb_generic_parse(struct parser** parser, const char* str)
{
	jsonb_parse_from_str(parser, str);
}


int main_tests(int argc, char* argv[])
{
	//char* test_jsonb_str_1 = "{\"test\": 123, \"testing\": 909, \"example\": \"a string\", \"array\": [1,2,3]}";
	//char* test_jsonb_str_1 = "[\"testing-example\", \"another-test\"]";
	//char* test_jsonb_str_1 = "[\"testing\", \"example\"]";
	//char* test_jsonb_str_1 = "[{ \"test\": \"value\", \"another\": 123 },, { \"nexttest\": \"another values\" }]";
	struct parser* parser;
	const char* test_jsonb_str_1 = "{\"test\": \"example\", \"another\": \"123\"}";	

	int status =  jsonb_parse_from_str(  &parser, test_jsonb_str_1);
	if ( status != 0 ) {
		printf("error reading JSON return int is %d\r\n", status);
		return -1;
	}

	//char* test_jsonb_str_2 = "[1, 2, 9]";
	//jsonb_parse_from_str( test_jsonb_str_2 );

	return 0;
}

int jsonb_token_cmp(char* token, const char* target)
{
    if (token == NULL) {
         return -1;
    }
    return strcmp( token, target );
}

int jsonb_token_end_cmp(char* token, const char* target)
{
    if (token == NULL) {
        return 0;
    }
    return strcmp( token, target );
}

int jsonb_token_int_cmp(struct parser* parser, char* token)
{
    if ( parser->opened_key.status == 0 ) {
        return -1;
    }
    if (token==NULL){
        return -1;
    }
    int result = jsonb_verify_int( token );
    if ( result == JSONB_CONTINUE_READ ) {
        return 0;
    }
    return -1;
}

int jsonb_token_str_cmp(struct parser* parser, char* token)
{
    if ( parser->opened_key.status == 0 ) {
        return -1;
    }
    if (token==NULL){
        return -1;
    }
    int result = jsonb_verify_str( token );
    if ( result == JSONB_CONTINUE_READ ) {
        return 0;
    }
    return -1;
}



int jsonb_validate_parent_end(struct parser* parser, char* token)
{
    if ( jsonb_parser_parent_end(parser, token) != 0 ) {
       return 0;
    }
    if (parser->code == JSONB_END_OF_READ) {
        return 0;
    }
    char* details;
    sprintf(details, "Illegal end of parser at index %d", parser->index);

    jsonb_parser_error(parser, details);
    return -1;
}

void jsonb_parser_error(struct parser* parser, char* details)
{
    puts(details);
    jsonb_set_code_error(parser);
}

void jsonb_parser_error_index(struct parser* parser, char* token)
{
    char* details;
    sprintf(details, "Illegal value '%s' at position %d", token, parser->index);
    jsonb_parser_error(parser,details);
}

     
void jsonb_printf(const char* fmt, ...)
{
#ifdef JSONB_DEBUG
	va_list vargs;
	va_start(vargs, fmt);
	vprintf(fmt, vargs);
	va_end(vargs);
#endif
}

