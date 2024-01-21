#include "s21_sprintf.h"

int s21_sprintf(char *str, const char *format, ...) {
  char *strPtr = str;
  va_list args;
  va_start(args, format);
  while (*format) {
    if (*format != '%') {
      *str++ = *format++;
      continue;
    } else {
      format++;
      Token token = parseTokenSP(&format, &args);
      handleTokenSP(token, &str, args, strPtr);
    }
  }
  *str = '\0';
  va_end(args);
  return str - strPtr;
}

Token parseTokenSP(const char **format, va_list *args) {
  Token tokenVar = {0};
  parseFlags(&tokenVar, format);
  parseWidth(&tokenVar, format, args);
  parseAccurasy(&tokenVar, format, args);
  parseLength(&tokenVar, format);
  parseSpec(&tokenVar, format);
  return tokenVar;
}

void parseFlags(Token *token, const char **format) {
  for (; **format == '0' || **format == '-' || **format == '+' ||
         **format == '#' || **format == ' ';
       (*format)++) {
    switch (**format) {
      case '0':
        token->Flags.NULL_F = true;
        break;
      case '+':
        token->Flags.PLUS_F = true;
        break;
      case '-':
        token->Flags.MINUS_F = true;
        break;
      case '#':
        token->Flags.SHARP_F = true;
        break;
      case ' ':
        token->Flags.SPACE_F = true;
        break;
    }
  }
}

void parseWidth(Token *token, const char **format, va_list *args) {
  if (**format == '*') {
    (*format)++;
    token->Width = va_arg(*args, int);
  } else if (isBasedDigit(**format, 10)) {
    s21_size_t widthValue = 0;
    for (; isBasedDigit(**format, 10); (*format)++) {
      widthValue = widthValue * 10 + (**format - '0');
    }
    token->Width = widthValue;
  }
  if (**format == '.') {
    (*format)++;
    token->Flags.ACCURACY = true;
  }
}

void parseAccurasy(Token *token, const char **format, va_list *args) {
  if (**format == '*') {
    (*format)++;
    token->Accuracy = va_arg(*args, int);
  } else if (isBasedDigit(**format, 10)) {
    s21_size_t accuracyValue = 0;
    for (; isBasedDigit(**format, 10); (*format)++) {
      accuracyValue = accuracyValue * 10 + (**format - '0');
    }
    token->Accuracy = accuracyValue;
  }
}

void parseLength(Token *token, const char **format) {
  while (**format == 'l' || **format == 'L' || **format == 'h') {
    switch (**format) {
      case 'l':
        token->LType = LONG_L;
        break;
      case 'L':
        token->LType = LONG_DOUBLE_L;
        break;
      case 'h':
        token->LType = SHORT_L;
        break;
    }
    (*format)++;
  }
}

void parseSpec(Token *token, const char **format) {
  token->Spec = **format;
  (*format)++;
}

void handleTokenSP(Token token, char **str, va_list args, char *strPtr) {
  char *buff = calloc(BUFF_SIZE, 1);
  bool nullChar = false;
  if (token.Spec == 'd' || token.Spec == 'i') {
    parseIntSP(&token, &buff, args);
  } else if (token.Spec == 'u') {
    parseUnsignedSP(&token, &buff, args);
  } else if (token.Spec == 'x' || token.Spec == 'X') {
    parseHexSP(&token, &buff, args);
  } else if (token.Spec == 'f') {
    parseFloatSP(&token, &buff, args);
  } else if (token.Spec == 'e' || token.Spec == 'E') {
    parseExpSP(&token, &buff, args);
  } else if (token.Spec == 'g' || token.Spec == 'G') {
    parseSciSP(&token, &buff, args);
  } else if (token.Spec == 's') {
    parseStrSP(&token, &buff, args);
  } else if (token.Spec == 'c') {
    nullChar = parseCharSP(&token, &buff, args);
  } else if (token.Spec == 'p') {
    parsePointerSP(&token, &buff, args);
  } else if (token.Spec == 'o') {
    parseOctalSP(&token, &buff, args);
  } else if (token.Spec == 'n') {
    *(va_arg(args, int *)) = *str - strPtr;
  } else if (token.Spec == '%') {
    buff[0] = '%';
  }
  for (int i = 0; buff[i]; i++, (*str)++) {
    **str = buff[i];
  }
  if (nullChar) {
    **str = '\0';
    (*str)++;
  }
  free(buff);
}

void handleFlags(Token *token, char **buffer) {
  char tmpBuffer[BUFF_SIZE] = {0};
  bool minusSign = ((*buffer)[0] == '-') ? true : false;
  if ((token->Flags.PLUS_F && token->Spec != 'u')) {
    tmpBuffer[0] = minusSign ? '-' : '+';
    s21_strcpy(tmpBuffer + 1, minusSign ? (*buffer) + 1 : *buffer);
    s21_strcpy(*buffer, tmpBuffer);
    minusSign = true;
  } else if ((token->Flags.SPACE_F && (*buffer)[0] != '-' &&
              token->Spec != 'u')) {
    tmpBuffer[0] = ' ';
    s21_strcpy(tmpBuffer + 1, *buffer);
    s21_strcpy(*buffer, tmpBuffer);
    minusSign = true;
  } else {
    s21_strcpy(tmpBuffer, *buffer);
  }
  s21_size_t buffSize = s21_strlen(*buffer);
  if (token->Width > (int)buffSize) {
    int diff = token->Width - buffSize;
    if (token->Flags.MINUS_F) {
      s21_strcpy(tmpBuffer, *buffer);
      s21_memset(tmpBuffer + s21_strlen(tmpBuffer), ' ', diff);
    } else {
      bool zeros = token->Flags.NULL_F;
      if (minusSign && tmpBuffer[0] != '+' && tmpBuffer[0] != ' ') {
        tmpBuffer[0] = '-';
      };
      s21_memset(zeros ? minusSign ? tmpBuffer + 1 : tmpBuffer : tmpBuffer,
                 zeros ? '0' : ' ', diff);
      s21_strcpy(zeros ? minusSign ? tmpBuffer + diff + 1 : tmpBuffer + diff
                       : tmpBuffer + diff,
                 zeros ? minusSign ? (*buffer) + 1 : *buffer : *buffer);
    }
  }
  if (s21_strlen(tmpBuffer) != 0) {
    s21_strcpy((*buffer), tmpBuffer);
  }
}

void handleAccuracy(Token *token, char **buffer) {
  char *tempBuffer = calloc(BUFF_SIZE, 1);
  bool negative = false;
  s21_size_t buffSize = s21_strlen(*buffer);
  if ((*buffer)[0] == '-') {
    tempBuffer[0] = '-';
    negative = true;
    buffSize--;
  }
  if (token->Accuracy > (int)buffSize) {
    int index = negative;
    for (; index < (int)(token->Accuracy - buffSize + negative); index++) {
      tempBuffer[index] = '0';
    }
    for (int i = negative; (*buffer)[i]; i++, index++) {
      tempBuffer[index] = (*buffer)[i];
    }
    s21_strcpy(*buffer, tempBuffer);
  }
  free(tempBuffer);
}

void handleSciAccurracy(char **buffer, int accuracy) {
  int digitCount = 0;
  s21_size_t strLen = s21_strlen(*buffer);
  int notZeroNotFound = 0;
  for (s21_size_t index = 0; index < strLen; index++) {
    if (((*buffer)[index] == '0' && !notZeroNotFound) ||
        (*buffer)[index] == '.')
      continue;
    else
      notZeroNotFound = 1;
    if (isBasedDigit((*buffer)[index], 10) && notZeroNotFound) {
      digitCount++;
    }
    if (digitCount == accuracy && index + 1 < strLen) {
      int offset = (*buffer)[index + 1] == '.' ? 2 : 1;
      if ((*buffer)[index + offset] - '0' >= 5) {
        (*buffer)[index] = (char)(*buffer)[index] + 1;
        if ((*buffer)[index] == ':') {
          (*buffer)[index - 1] = (char)(*buffer)[index - 1] + 1;
          (*buffer)[index] = '0';
        }
      }
      (*buffer)[index + 1] = '\0';
      break;
    }
  }
}

void parseIntSP(Token *token, char **buffer, va_list args) {
  int64_t val = va_arg(args, int64_t);
  if (token->LType == SHORT_L) {
    val = (int16_t)val;
  } else if (token->LType == NONE_L) {
    val = (int32_t)val;
  }
  if (val == 0 && token->Flags.ACCURACY && token->Accuracy == 0) {
    **buffer = '\0';
  } else {
    numToBuffer(val, 10, buffer);
    reverse(buffer);
    handleAccuracy(token, buffer);
    handleFlags(token, buffer);
  }
}

void parseUnsignedSP(Token *token, char **buffer, va_list args) {
  uint64_t val = va_arg(args, uint64_t);
  if (token->LType == SHORT_L) {
    val = (uint16_t)val;
  } else if (token->LType == NONE_L) {
    val = (uint32_t)val;
  }
  unsignedToBuffer(val, 10, buffer);
  reverse(buffer);
  handleAccuracy(token, buffer);
  handleFlags(token, buffer);
}

void parseStrSP(Token *token, char **buffer, va_list args) {
  char temp[BUFF_SIZE] = {0};
  char *strVal = va_arg(args, char *);
  if (strVal == S21_NULL) {
    s21_strcpy(*buffer, "(null)");
  } else {
    s21_strcpy(temp, strVal);
    if (token->Accuracy) {
      temp[token->Accuracy] = 0;
    }
    s21_strcpy(*buffer, temp);
    token->Flags.NULL_F = token->Flags.PLUS_F = 0;
    token->Flags.SHARP_F = token->Flags.SPACE_F = 0;
    handleFlags(token, buffer);
  }
}

bool parseCharSP(Token *token, char **buffer, va_list args) {
  bool isCharZero = false;
  char symbol;
  symbol = va_arg(args, int);
  if (symbol == 0) {
    isCharZero = true;
  }
  (*buffer)[0] = symbol;
  token->Flags.NULL_F = token->Flags.PLUS_F = 0;
  token->Flags.SHARP_F = token->Flags.SPACE_F = 0;
  handleFlags(token, buffer);
  return isCharZero;
}

void parsePointerSP(Token *token, char **buffer, va_list args) {
  uint64_t pointerValue = va_arg(args, uint64_t);
  if (pointerValue == 0) {
    s21_strcpy(*buffer, "(nil)");
  } else {
    unsignedToBuffer(pointerValue, 16, buffer);
    reverse(buffer);
    handleAccuracy(token, buffer);
    reverse(buffer);
    s21_size_t bufferSize = s21_strlen(*buffer);
    (*buffer)[bufferSize + 1] = '0';
    (*buffer)[bufferSize] = 'x';
    reverse(buffer);
    token->Flags.NULL_F = token->Flags.SHARP_F = 0;
    handleFlags(token, buffer);
  }
}

void parseHexSP(Token *token, char **buffer, va_list args) {
  uint64_t hexValue = va_arg(args, uint64_t);
  if (token->LType == NONE_L) {
    hexValue = (uint32_t)hexValue;
  } else if (token->LType == SHORT_L) {
    hexValue = (uint16_t)hexValue;
  }
  unsignedToBuffer(hexValue, 16, buffer);
  reverse(buffer);
  handleAccuracy(token, buffer);
  bool hasNonZeroDigits = false;
  for (s21_size_t i = 0; i < s21_strlen(*buffer); ++i) {
    if ((*buffer)[i] != '0') {
      hasNonZeroDigits = true;
      break;
    }
  }
  if (hasNonZeroDigits && token->Flags.SHARP_F) {
    reverse(buffer);
    s21_size_t bufferSize = s21_strlen(*buffer);
    (*buffer)[bufferSize + 1] = '0';
    (*buffer)[bufferSize] = 'x';
    reverse(buffer);
  }
  token->Flags.PLUS_F = token->Flags.SPACE_F = 0;
  handleFlags(token, buffer);
  if (token->Spec == 'X') {
    toUpperSP(buffer);
  }
}

void parseOctalSP(Token *token, char **buffer, va_list args) {
  uint64_t octalValue = va_arg(args, uint64_t);
  numToBuffer(octalValue, 8, buffer);
  s21_size_t bufferSize = s21_strlen(*buffer);
  if (token->Flags.SHARP_F) {
    (*buffer)[bufferSize] = '0';
  }
  reverse(buffer);
  handleAccuracy(token, buffer);
  token->Flags.PLUS_F = 0;
  token->Flags.SPACE_F = 0;
  handleFlags(token, buffer);
}

void parseFloatSP(Token *token, char **buffer, va_list args) {
  long double floatValue = 0;
  if (token->LType == LONG_DOUBLE_L) {
    floatValue = va_arg(args, long double);
  } else {
    floatValue = va_arg(args, double);
  }
  if (!token->Flags.ACCURACY) {
    token->Accuracy = 6;
  }
  floatToBuffer(token, floatValue, buffer);
  handleFlags(token, buffer);
}

void parseExpSP(Token *token, char **buffer, va_list args) {
  long double expValue = 0;
  if (token->LType == LONG_DOUBLE_L) {
    expValue = va_arg(args, long double);
  } else {
    expValue = va_arg(args, double);
  }
  int expCounter = 0, sign = (int)expValue == 0 ? '-' : '+';
  double fractionalPart = (double)((int)expValue - expValue);
  if (fabs(fractionalPart) > eps) {
    for (; (long long int)expValue == 0; expValue *= 10) expCounter++;
  } else {
    sign = '+';
  }
  for (; (long long int)expValue / 10 != 0; expValue /= 10) expCounter++;
  if (!token->Flags.ACCURACY) {
    token->Accuracy = 6;
  }
  floatToBuffer(token, expValue, buffer);
  expToBuffer(buffer, sign, expCounter);
  if (token->Spec == 'E') {
    toUpperSP(buffer);
  }
  handleFlags(token, buffer);
}

void parseSciSP(Token *token, char **buffer, va_list args) {
  long double val = 0;
  if (token->LType == LONG_DOUBLE_L) {
    val = va_arg(args, long double);
  } else {
    val = va_arg(args, double);
  }
  int posE = 0, negE = 0;
  if (!token->Flags.ACCURACY) {
    token->Accuracy = 6;
  }
  if (!token->Accuracy) {
    token->Accuracy = 1;
  }
  long double intPart = val;
  for (; (int)intPart / 10; intPart /= 10, posE++) {
  };
  long double floatPart = val;
  if (!posE) {
    if (((int)val - val) > eps || ((int)val - val) < -eps) {
      for (; !((int)floatPart); negE++, floatPart *= 10) {
      }
    }
    floatPart = roundl(floatPart * 10e+10) / 10e10;
  }
  if (posE >= token->Accuracy) {
    floatToBuffer(token, intPart, buffer);
  } else if (negE > 4) {
    floatToBuffer(token, floatPart, buffer);
  } else if (token->Flags.ACCURACY && negE > token->Accuracy) {
    token->Accuracy = negE;
    floatToBuffer(token, val, buffer);
  } else {
    floatToBuffer(token, val, buffer);
  }
  handleSciAccurracy(buffer, token->Accuracy);
  if (!token->Flags.SHARP_F) removeNulls(token, buffer);
  if (posE >= token->Accuracy) {
    expToBuffer(buffer, '+', posE);
  } else if (negE > 4) {
    expToBuffer(buffer, '-', negE);
  }
  if (token->Spec == 'G') {
    toUpperSP(buffer);
  }
  handleFlags(token, buffer);
}

void floatToBuffer(Token *token, long double num, char **buffer) {
  char *bufferInt = calloc(BUFF_SIZE, 1);
  char *bufferFloat = calloc(BUFF_SIZE, 1);
  bool neg = num < -1e-15 ? true : false;
  num = neg ? -num : num;
  long double intPart = 0, floatPart = modfl(num, &intPart);
  floatPart = token->Accuracy ? floatPart : 0;
  intPart = token->Accuracy ? intPart : roundl(num);
  int i = 0;
  for (int j = 0; j < token->Accuracy; j++) {
    floatPart *= 10;
  }
  long long right = (long long int)roundl(floatPart);
  if ((token->Flags.ACCURACY && token->Accuracy != 0) ||
      (!token->Flags.ACCURACY && (num < eps && num > -eps)) ||
      (long long int)floatPart || token->Accuracy || token->Flags.SHARP_F) {
    bufferInt[i] = '.', i++;
  }
  if (intPart > eps || intPart < -eps) {
    for (; (long long int)intPart != 0; i++, intPart /= 10) {
      bufferInt[i] = "0123456789"[(long long int)intPart % 10];
    }
  } else {
    bufferInt[i] = '0', i += 1;
  }
  if (neg) {
    bufferInt[i] = '-';
  };
  reverse(&bufferInt);
  for (int j = 0; j < token->Accuracy || right; j++, right /= 10) {
    bufferFloat[j] = "0123456789"[(long long int)(right % 10 + 0.001)];
  }
  reverse(&bufferFloat);
  bufferInt = s21_strcat(bufferInt, bufferFloat);
  s21_strcpy(*buffer, bufferInt);
  free(bufferInt);
  free(bufferFloat);
}

void numToBuffer(int64_t num, int base, char **buffer) {
  char *temp = *buffer;
  bool negative = false;
  if (num < 0) {
    negative = true;
    num *= -1;
  }
  if (num == 0) {
    *temp = '0';
    temp++;
  }
  while (num != 0) {
    *temp = "0123456789abcdef"[num % base];
    num /= base;
    temp++;
  }
  if (negative) {
    *temp = '-';
  };
}

void expToBuffer(char **buffer, int sign, int e) {
  s21_size_t size = s21_strlen(*buffer);
  (*buffer)[size] = 'e';
  (*buffer)[size + 1] = sign;
  (*buffer)[size + 2] = ("0123456789"[(e / 10) % 10]);
  (*buffer)[size + 3] = ("0123456789"[e % 10]);
}

void removeNulls(Token *token, char **buffer) {
  int len = s21_strlen(*buffer);
  char *dot = S21_NULL;
  dot = s21_strchr(*buffer, '.');
  if (dot) {
    for (int i = len - 1; *(*buffer + i) != '.'; i--) {
      if ((*buffer)[i] == '0') {
        (*buffer)[i] = '\0';
      } else {
        break;
      }
    }
    if (!token->Flags.SHARP_F && *(dot + 1) == '\0') *dot = '\0';
  }
}

void unsignedToBuffer(uint64_t num, int base, char **buffer) {
  char *temp = *buffer;
  if (num == 0) {
    *temp = '0';
    temp++;
  }
  while (num != 0) {
    *temp = "0123456789abcdef"[num % base];
    num /= base;
    temp++;
  }
}

void reverse(char **str) {
  char temp[BUFF_SIZE] = {0};
  int size = (int)s21_strlen(*str);
  for (int i = size - 1; i >= 0; i--) {
    temp[size - i - 1] = (*str)[i];
  }
  for (int i = 0; i < size; i++) {
    (*str)[i] = temp[i];
  }
}

void toUpperSP(char **str) {
  s21_size_t size = s21_strlen(*str);
  for (s21_size_t i = 0; i < size; i++) {
    if ((*str)[i] >= 'a' && (*str)[i] <= 'z') {
      (*str)[i] -= 32;
    }
  }
}
