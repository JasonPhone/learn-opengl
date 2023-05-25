#pragma once
#include <stdio.h>

#include <cassert>
#include <string>


inline void string_format_recursive(std::string *s, const char *fmt) {
  const char *c = fmt;
  // Make sure no extra formatting specifiers when no args left.
  while (*c) {
    if (*c == '%') {
      assert(c[1] == '%');
      ++c;
    }
    *s += *c++;
  }
}

/**
 * @brief 1. Copy from fmt to *s, up to the next formatting directive.
 *        2. Advance fmt past the next formatting directive and return it
 *           as a string.
 *
 * @param fmt_ptr const char**, to modify the original fmt.
 * @param s To store the non-format part.
 */
inline std::string next_format(const char **fmt_ptr, std::string *s) {
  // Reference to a pointer.
  const char *&fmt = *fmt_ptr;
  while (*fmt) {
    if (*fmt != '%') {
      *s += *fmt;
      ++fmt;
    } else if (fmt[1] == '%') {
      // "%%", skip.
      *s += '%';
      *s += '%';
      fmt += 2;
    } else {
      // We find the start of a formatting directive.
      break;
    }
  }
  std::string next_fmt;
  if (*fmt) {
    /**
     * Take chars behind the '%' until:
     *   empty, potential new formatting, space, comma, brackets.
     * Incomplete but enough for INTERNAL USE.
     */
    do {
      next_fmt += *fmt;
      ++fmt;
    } while (*fmt && *fmt != '%' && !isspace(*fmt) && *fmt != ',' &&
             *fmt != '[' && *fmt != ']' && *fmt != '(' && *fmt != ')' &&
             *fmt != '{' && *fmt != '}' && *fmt != '<' && *fmt != '>');
  }
  return next_fmt;
}

template <typename T>
inline std::string format_one(const char *fmt, T v) {
  // First call to count the required space. Extra one for '\0'.
  size_t size = snprintf(nullptr, 0, fmt, v) + 1;
  std::string str;
  str.resize(size);
  // Second call to fill the string.
  snprintf(&str[0], size, fmt, v);
  str.pop_back();  // remove '\0'
  return str;
}

/**
 * @brief General-purpose version of string_format_recursive.
 *        Format the string into @param s one arg each iteration.
 */
template <typename T, typename... Args>
inline void string_format_recursive(std::string *s, const char *fmt, T v,
                                    Args... args) {
  std::string next_fmt = next_format(&fmt, s);
  *s += format_one(next_fmt.c_str(), v);
  string_format_recursive(s, fmt, args...);
}

/// @brief Special case of string_format_recursive for float.
template <typename... Args>
inline void string_format_recursive(std::string *s, const char *fmt, float v,
                                    Args... args) {
  std::string next_fmt = next_format(&fmt, s);
  if (next_fmt == "%f")
    // Always use enough precision.
    // https://randomascii.wordpress.com/2012/03/08/float-precisionfrom-zero-to-100-digits-2/
    // *s += format_one("%.9g", v);
    *s += format_one("%.4g", v);
  else
    // No operation.
    *s += format_one(next_fmt.c_str(), v);
  // Go forth for next arg.
  string_format_recursive(s, fmt, args...);
}

/// @brief Special case of string_format_recursive for float.
template <typename... Args>
inline void string_format_recursive(std::string *s, const char *fmt, double v,
                                    Args... args) {
  std::string next_fmt = next_format(&fmt, s);
  if (next_fmt == "%f")
    // *s += format_one("%.17g", v);
    *s += format_one("%.4g", v);
  else
    *s += format_one(next_fmt.c_str(), v);
  string_format_recursive(s, fmt, args...);
}

/**
 * @brief A C++ version of sprintf() returning std::string.
 *        Floats (formatted by "%f" only) will be special treated for enough
 *        precision.
 */
template <typename... Args>
inline std::string string_format(const char *fmt, Args... args) {
  std::string ret;
  string_format_recursive(&ret, fmt, args...);
  return ret;
}