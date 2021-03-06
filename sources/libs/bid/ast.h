#pragma once

#include <bid/bid.h>
#include <bid/types.h>
#include <brutal/ds.h>

typedef struct
{
    Str name;
    BidType type;
} BidAlias;

typedef struct
{
    Str name;

    BidType request;
    BidType response;
} BidMethod;

typedef struct
{
    Str name;
    BidType data;
} BidEvent;

typedef struct
{
    Str name;
    uint32_t id;

    BidEnum errors;
    Vec(BidAlias) aliases;
    Vec(BidEvent) events;
    Vec(BidMethod) methods;
} BidInterface;
