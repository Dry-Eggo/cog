#ifndef __COG_STRING_H__
#define __COG_STRING_H__

#include "cog.h"

cog_string cog_string__init(cog_cstr str);
cog_u64 cog_string__len(cog_string* self);
cog_u64 cog_string__capacity(cog_string* self);


#endif
