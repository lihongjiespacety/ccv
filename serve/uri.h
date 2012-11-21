#ifndef _GUARD_uri_h_
#define _GUARD_uri_h_

#include "ebb.h"

typedef enum {
	s_form_data_start,
	s_form_data_header_field,
	s_form_data_header_value_start,
	s_form_data_header_value_name_start,
	s_form_data_header_value_name_done,
	s_form_data_end,
} form_data_state_t;

typedef struct {
	form_data_state_t state;
	char lookbehind;
	int cursor;
	int disposition_index;
	void* context;
	void (*on_name)(void*, const char*, size_t);
} form_data_parser_t;

void form_data_parser_init(form_data_parser_t* parser, void* context);
void form_data_parser_execute(form_data_parser_t* parser, const char* buf, size_t len, int header_index);

typedef enum {
	s_numeric_start,
	s_numeric_before_decimal,
	s_numeric_after_decimal,
	s_numeric_illegal,
} numeric_state_t;

typedef struct {
	numeric_state_t state;
	double result;
	double division;
} numeric_parser_t;

void numeric_parser_init(numeric_parser_t* parser);
void numeric_parser_execute(numeric_parser_t* parser, const char* buf, size_t len);

typedef enum {
	s_bool_start,
	s_bool_1,
	s_bool_0,
	s_bool_true,
	s_bool_false,
	s_bool_illegal,
} bool_state_t;

typedef struct {
	bool_state_t state;
	int cursor;
	int result;
} bool_parser_t;

void bool_parser_init(bool_parser_t* parser);
void bool_parser_execute(bool_parser_t* parser, const char* buf, size_t len);

typedef enum {
	s_coord_start,
	s_coord_illegal,
} coord_state_t;

typedef struct {
	coord_state_t state;
	double x;
	double y;
} coord_parser_t;

void coord_parser_init(coord_parser_t* parser);
void coord_parser_execute(coord_parser_t* parser, const char* buf, size_t len);

typedef enum {
	s_string_start,
	s_string_overflow,
} string_state_t;

typedef struct {
	string_state_t state;
	char string[16];
	int cursor;
} string_parser_t;

void string_parser_init(string_parser_t* parser);
void string_parser_execute(string_parser_t* parser, const char* buf, size_t len);

typedef enum {
	URI_QUERY_STRING,
	URI_MULTIPART_HEADER_FIELD,
	URI_MULTIPART_HEADER_VALUE,
	URI_MULTIPART_DATA,
	URI_PARSE_TERMINATE,
} uri_parse_state_t;

typedef struct {
	char* uri;
	void* context;
	void* (*init)(void); // this runs on server start
	void* (*parse)(const void*, void*, const char*, size_t, uri_parse_state_t, int); // this runs on main thread
	ebb_buf (*get)(const void*, const void*); // this runs off thread
	ebb_buf (*post)(const void*, const void*); // this runs off thread
} uri_dispatch_t;

uri_dispatch_t* find_uri_dispatch(const char* path);
void uri_init(void);

ebb_buf uri_root_discovery(const void* context, const void* parsed);

void* uri_bbf_detect_objects_init(void);
void* uri_bbf_detect_objects_parse(const void* context, void* parsed, const char* buf, size_t len, uri_parse_state_t state, int header_index);
ebb_buf uri_bbf_detect_objects_intro(const void* context, const void* parsed);
ebb_buf uri_bbf_detect_objects(const void* context, const void* parsed);

#endif