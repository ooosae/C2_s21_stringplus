#include "s21_string.h"

int s21_memcmp(const void *str1, const void *str2, s21_size_t n) {
  int res = 0;
  char *p_str1 = (char *)str1;
  char *p_str2 = (char *)str2;
  if (p_str1 != p_str2) {
    for (s21_size_t i = 0; i < n; i++) {
      if (*(p_str1 + i) != *(p_str2 + i)) {
        res = *(p_str1 + i) - *(p_str2 + i);
        break;
      }
    }
  }
  return res;
}

int s21_strcmp(const char *str1, const char *str2) {
  while (*str1 && (*str1 == *str2)) {
    str1++;
    str2++;
  }
  return (*(unsigned char *)str1 - *(unsigned char *)str2);
}

int s21_strncmp(const char *str1, const char *str2, s21_size_t n) {
  s21_size_t iterator = 0;
  while (iterator < n && *str1 && (*str1 == *str2)) {
    str1++;
    str2++;
    iterator++;
  }
  return (iterator < n) ? (*(unsigned char *)str1 - *(unsigned char *)str2) : 0;
}

void *s21_memchr(const void *str, int c, s21_size_t n) {
  char *temp_str = (char *)str;
  while (n-- && *temp_str != (char)c) {
    temp_str++;
  }
  return ((int)n == -1) ? S21_NULL : temp_str;
}

void *s21_memcpy(void *dest, const void *src, s21_size_t n) {
  char *temp_dest = (char *)dest;
  const char *temp_src = (const char *)src;
  while (n--) {
    *temp_dest++ = *temp_src++;
  }
  return dest;
}

void *s21_memmove(void *dest, const void *src, s21_size_t n) {
  void *res = dest;
  if (dest != src && n != 0) {
    char *temp_src = (char *)src;
    char *temp_dest = (char *)dest;
    for (s21_size_t iterator = 0; iterator < n; iterator++) {
      temp_dest[iterator] = temp_src[iterator];
    }
  }
  return res;
}

void *s21_memset(void *str, int c, s21_size_t n) {
  char *temp_str = (char *)str;
  while (n--) {
    *temp_str++ = (char)c;
  }
  return str;
}

char *s21_strcat(char *dest, const char *src) {
  char *temp_dest = dest;
  while (*temp_dest) {
    temp_dest++;
  }
  while ((*temp_dest++ = *src++)) {
  }
  return dest;
}

char *s21_strncat(char *dest, const char *src, s21_size_t n) {
  char *res = dest;
  if (dest != S21_NULL && src != S21_NULL) {
    while (*dest != '\0') {
      dest++;
    }
    for (s21_size_t iterator = 0; iterator < n; iterator++) {
      *dest = src[iterator];
      dest++;
    }
  }
  return res;
}

char *s21_strchr(const char *str, int c) {
  char *res;
  int flag = 0;
  s21_size_t iterator = 0;
  while (1) {
    if (str[iterator] == (char)c ||
        (str[iterator] == '\0' && (char)c == '\0')) {
      flag = 1;
      break;
    } else if (str[iterator] == '\0') {
      break;
    }
    iterator++;
  }
  if (flag == 0) {
    res = S21_NULL;
  } else {
    res = (char *)str + iterator;
  }
  return res;
}

char *s21_strcpy(char *dest, const char *src) {
  char *res = dest;
  while ((*dest++ = *src++)) {
  }
  return res;
}

char *s21_strncpy(char *dest, const char *src, s21_size_t n) {
  char *res = dest;
  while (n && (*dest = *src)) {
    dest++;
    src++;
    n--;
  }
  return res;
}

char *s21_strerror(int errnum) {
  static char res[MAX_ERROR_LEN];
  if (errnum < 0 || errnum >= MAX_ERROR || ERRLIST[errnum] == NULL) {
    s21_sprintf(res, "Unknown error %d", errnum);
  } else {
    s21_strncpy(res, ERRLIST[errnum], MAX_ERROR_LEN - 1);
  }
  return res;
}

char *s21_strpbrk(const char *str1, const char *str2) {
  char *res = S21_NULL;
  while (*str1) {
    const char *ptr = str2;
    while (*ptr) {
      if (*str1 == *ptr++) {
        res = (char *)str1;
        break;
      }
    }
    if (res != S21_NULL) {
      break;
    }
    str1++;
  }
  return res;
}

char *s21_strrchr(const char *str, int c) {
  char *res = S21_NULL;
  if (str != S21_NULL) {
    if (c == '\0') {
      res = s21_strchr(str, '\0');
    } else {
      char *temp = (char *)str;
      s21_size_t size = s21_strlen(str);
      while (size != 0) {
        size--;
        if (temp[size] == c) {
          res = &temp[size];
          break;
        }
      }
    }
  }
  return res;
}

char *s21_strstr(const char *haystack, const char *needle) {
  char *result = S21_NULL;
  int haystack_len = s21_strlen(haystack);
  int needle_len = s21_strlen(needle);
  if (haystack_len >= needle_len) {
    s21_size_t i = 0;
    while (i < s21_strlen(haystack)) {
      s21_size_t match_count = 0;
      s21_size_t j = 0;
      while (j < s21_strlen(needle)) {
        if (haystack[i] == needle[0] && haystack[i + j] == needle[j]) {
          match_count += 1;
        }
        j++;
      }
      if (match_count == s21_strlen(needle)) {
        result = (char *)haystack + i;
        break;
      }
      i++;
    }
  }
  return result;
}

char *s21_strtok(char *str, const char *delim) {
  static char *static_str = S21_NULL;
  if (str == S21_NULL) str = static_str;
  for (; str && *str && s21_strchr(delim, *str); str++) {
  }
  if (str && *str == '\0') {
    str = S21_NULL;
    static_str = S21_NULL;
  }
  if (str && *str != '\0') {
    static_str = str;
    for (; *static_str && !(s21_strchr(delim, *static_str)); static_str++) {
    }
  }
  if (static_str && *static_str) {
    *static_str = '\0';
    static_str++;
  }
  if (static_str && *static_str == '\0') {
    static_str = S21_NULL;
  }
  return str;
}

s21_size_t s21_strcspn(const char *str1, const char *str2) {
  s21_size_t count = 0;
  for (; *str1 && !s21_strchr(str2, *str1); str1++, count++)
    ;
  return count;
}

s21_size_t s21_strlen(const char *str) {
  s21_size_t count = 0;
  for (; str[count] != '\0'; count++)
    ;
  return count;
}

s21_size_t s21_strspn(const char *str1, const char *str2) {
  s21_size_t count = 0;
  for (; *str1 && s21_strchr(str2, *str1); str1++, count++)
    ;
  return count;
}

void *s21_to_upper(const char *str) {
  char *output = S21_NULL;
  s21_size_t length = s21_strlen(str);
  output = (char *)calloc(length + 1, 1);
  if (output != S21_NULL) {
    s21_strcpy(output, str);
    for (s21_size_t index = 0; index < length + 1; index++) {
      if (output[index] >= 97 && output[index] <= 122) {
        output[index] -= 32;
      }
    }
  }
  return (void *)output;
}

void *s21_to_lower(const char *str) {
  char *output = S21_NULL;
  s21_size_t length = s21_strlen(str);
  output = (char *)calloc(length + 1, 1);
  if (output != S21_NULL) {
    s21_strcpy(output, str);
    for (s21_size_t index = 0; index < length + 1; index++) {
      if (output[index] >= 65 && output[index] <= 90) {
        output[index] += 32;
      }
    }
  }
  return (void *)output;
}

void *s21_insert(const char *src, const char *str, s21_size_t start_index) {
  char *output = S21_NULL;
  if (src && str && start_index <= (s21_strlen(src))) {
    s21_size_t src_len = s21_strlen(src), str_len = s21_strlen(str);
    output = (char *)calloc(src_len + str_len + 1, 1);
    if (output != S21_NULL) {
      s21_size_t i = 0;
      while (i < start_index) {
        output[i] = src[i];
        i++;
      }
      for (i = start_index; i < str_len + start_index; i++) {
        output[i] = str[i - start_index];
      }
      while (i < str_len + src_len) {
        output[i] = src[i - str_len];
        i++;
      }
    }
  }
  return (void *)output;
}

void *s21_trim(const char *src, const char *trim_chars) {
  char *result = S21_NULL;
  if (src != S21_NULL) {
    if (trim_chars == S21_NULL) trim_chars = "";
    s21_size_t left = s21_strspn(src, trim_chars);
    char *revert_string = s21_revstr(src);
    s21_size_t right = s21_strspn(revert_string, trim_chars);
    s21_size_t src_len = (src == S21_NULL) ? 0 : s21_strlen(src);
    s21_size_t len = (src_len < (right + left)) ? 0 : src_len - (right + left);
    result = calloc(len + 1, sizeof(char));
    if (result != S21_NULL) {
      s21_memmove(result, &src[left], len);
      result[len] = '\0';
    }
    free(revert_string);
  }
  return result;
}

char *s21_revstr(const char *str) {
  s21_size_t len = (str == S21_NULL) ? 0 : s21_strlen(str);
  s21_size_t i = 0;
  char *result = calloc((s21_size_t)(len + 1), sizeof(char));
  if (result != S21_NULL) {
    while (str[i]) {
      len--;
      result[i] = str[len];
      i++;
    }
    result[i] = '\0';
  }
  return result;
}
