#pragma once

#define EN_ASSERT(cond, ...)\
if(!(cond)) {\
	EN_LOG_ERROR(__VA_ARGS__);\
    EN_DEBUG_BREAK();\
}\
