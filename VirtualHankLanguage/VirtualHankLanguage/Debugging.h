#pragma once

#define _DEPRECATE(_text) __declspec(deprecated(_text))
#define DEPRECATE __declspec(deprecated("Function has been depricated."))
#define DEPRECATE_REPLACE(_replacement) _DEPRECATE("Function has been depricated. Please use '" #_replacement "' instead.")

#define ASSERT(cond,message) _ASSERT_EXPR(cond,message);
