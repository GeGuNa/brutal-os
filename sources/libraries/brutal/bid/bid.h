#pragma once
#include <brutal/base.h>
#include <brutal/ds.h>
#include <brutal/io/scan.h>

#define BID_INTERFACE_STR (str_cast("interface"))
#define BID_ALNUM_STR (str_cast("[a-Az-Z_] string"))
#define BID_CLOSING_BRACKETS (str_cast("}"))
#define BID_OPENNING_BRACKETS (str_cast("}"))

enum bid_error_type
{
    BID_ERROR_TYPE_INVALID,
    BID_ERROR_TYPE_UNEXPECTED_TOKEN,
};

struct bid_buffer_position
{
    int offset;
};

struct bid_error
{
    enum bid_error_type type;
    struct bid_buffer_position pos;
    Str message;

    union
    {
        struct
        {
            Str name;
        } expected_token;
    } specific_information;
};

enum bid_ast_node_type
{
    BID_AST_NODE_TYPE_UNKNOWN,
    BID_AST_NODE_TYPE_INTERFACE,
};

struct bid_ast_node
{
    enum bid_ast_node_type type;
    Vec(struct bid_ast_node) children;

    union
    {
        struct ast_node_interface
        {
            Str name;
        } interface;
    };
};

struct bid
{
    Str in_data;

    struct bid_ast_node *root_ast;
    struct scan scanner;
    int _current_scanned_token_cursor;
};

typedef Result(struct bid_error, MonoState) BidParseResult;
typedef Result(struct bid_error, struct bid) BidResult;

BidResult init_bid(Str idl_in);

void destroy_bid(struct bid *in);

enum bid_token_type bid_token_from_char(char from);

enum bid_keywords bid_keyword_from_string(Str from);

void print_ast_node_recursive(struct bid_ast_node *from);