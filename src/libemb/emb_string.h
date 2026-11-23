#ifndef __EMB_STRING_H__
#define __EMB_STRING_H__

#include <libemb/emb_config.h>

// stricmp wrapper
#ifdef _MSC_VER
#include <string.h>
#define emb_stricmp _stricmp
#elif defined(__GNUC__) || defined(__clang__) || defined(__ARMCC_VERSION)
#include <string.h>
#define emb_stricmp strcasecmp
#else
#error "Platform not supported"
// TODO: maybe implement our own stricmp for other platforms?
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Convert a character to uppercase.
 * @param ch The character to convert.
 * @return The uppercase equivalent of the character if it's a lowercase letter; otherwise, returns the character unchanged.
 */
EMB_EXPORT_SYMBOL char emb_to_upper(char ch);

/**
 * @brief Convert a character to lowercase.
 * @param ch The character to convert.
 * @return The lowercase equivalent of the character if it's an uppercase letter; otherwise, returns the character unchanged.
 */
EMB_EXPORT_SYMBOL char emb_to_lower(char ch);

/**
 * @brief Compare two strings in a case-insensitive manner for a specified number of characters.
 * @param str1 The first string to compare.
 * @param str2 The second string to compare.
 * @param n The maximum number of characters to compare.
 * @return 0 if the strings are equal (case-insensitive) up to n characters, a negative value if str1 is less than str2, or a positive value if str1 is greater than str2.
 * @note The comparison is performed in a case-insensitive manner, meaning that uppercase and lowercase letters are considered equivalent.
 * @note If n is zero, the function returns 0, indicating that the strings are considered equal for the purpose of this comparison.
 * @note If either str1 or str2 is NULL, the function returns -1, indicating that the comparison cannot be performed. If both strings are NULL, the function returns 0, indicating that they are considered equal.
 * @note The function does not modify the input strings.
 */
EMB_EXPORT_SYMBOL int emb_strnicmp(const char* str1, const char* str2, size_t n);

/**
 * @brief Calculate the length of a string up to a maximum number of characters.
 * @param s The string whose length is to be calculated.
 * @param maxlen The maximum number of characters to examine.
 * @return The number of characters in the string, not including the terminating null character, but at most maxlen.
 */
EMB_EXPORT_SYMBOL size_t emb_strnlen(const char* s, size_t maxlen);

/**
 * @brief Check if a string starts with a specified prefix.
 * @param str The string to check.
 * @param prefix The prefix to look for.
 * @return true if str starts with prefix; otherwise, false.
 * @note If either str or prefix is NULL, the function returns false.
 */
EMB_EXPORT_SYMBOL bool emb_str_starts_with(const char* str, const char* prefix);

/**
 * @brief Check if a string ends with a specified suffix.
 * @param str The string to check.
 * @param suffix The suffix to look for.
 * @return true if str ends with suffix; otherwise, false.
 * @note If either str or suffix is NULL, the function returns false.
 */
EMB_EXPORT_SYMBOL bool emb_str_ends_with(const char* str, const char* suffix);

/**
 * @brief Check if a string contains a specified substring.
 * @param str The string to check.
 * @param substr The substring to look for.
 * @return true if str contains substr; otherwise, false.
 * @note If either str or substr is NULL, the function returns false.
 * @note An empty substring is considered to be contained in any string, including an empty string.
 */
EMB_EXPORT_SYMBOL bool emb_str_contains(const char* str, const char* substr);

// 0-9, A-F to '0'-'9', 'A'-'F'
// h will be masked with 0x0F, so it can be greater than 15, but only the lower 4 bits will be used
EMB_EXPORT_SYMBOL uint8_t emb_hex2char(uint8_t h);

// '0'-'9', 'A'-'F', 'a'-'f' to 0-9, A-F, return 0xFF if invalid
EMB_EXPORT_SYMBOL uint8_t emb_char2hex(uint8_t c);

#ifdef _MSC_VER
// msvc doesn't have strlcpy, so we provide our own (OPENBSD) implementation
EMB_EXPORT_SYMBOL size_t strlcpy(char* dst, const char* src, size_t dst_size);
#endif

/**
 * @brief Escape a string for JSON output.
 * @param str The input string to escape.
 * @param out_len Pointer to a size_t variable where the length of the escaped string will be stored. Can be NULL if not needed.
 * @return A newly allocated string containing the escaped version of the input string. The caller is responsible for freeing this string using `emb_free_json_escaped_string`. Returns NULL if memory allocation fails.
 * @note The returned string is NUL-terminated.
 */
EMB_EXPORT_SYMBOL char* emb_json_escape_string(const char* str, size_t* out_len);

/**
 * @brief Free a string allocated by `emb_json_escape_string`.
 * @param str The string to free.
 */
EMB_EXPORT_SYMBOL void emb_free_json_escaped_string(char* str);

/**
 * @brief Escape a string for JSON output into a provided buffer.
 * @param str The input string to escape.
 * @param buffer The buffer to store the escaped string.
 * @param buffer_size The size of the buffer.
 * @return The number of characters written to the buffer, excluding the NUL terminator.
 * @note The buffer will be NUL-terminated if there is enough space.
 * @note When `NULL` is provided for `buffer` or `buffer_size` is not enough, the function will calculate the required buffer size and return it without writing any data.
 */
EMB_EXPORT_SYMBOL size_t emb_json_escape_string_to_buffer(const char* str, char* buffer, size_t buffer_size);

/**
 * @brief Check if a buffer is filled with a specific byte value.
 * @param buf Pointer to the buffer to check.
 * @param len Length of the buffer in bytes.
 * @param value The byte value to check against.
 * @return true if all bytes in the buffer are equal to the specified value; otherwise, false.
 * @note If `buf` is NULL or `len` is 0, the function returns false.
 */
EMB_EXPORT_SYMBOL bool emb_buf_is_all(const uint8_t* buf, size_t len, uint8_t value);

#ifdef __cplusplus
}
#endif

#endif /* __EMB_STRING_H__ */
