#include "language.h"

static int fHexToDec(char hex)
{
  if (hex >= '0' && hex <= '9')
    return hex - '0';
  if (hex >= 'A' && hex <= 'F')
    return hex - 'A' + 10;
  if (hex >= 'a' && hex <= 'f')
    return hex - 'a' + 10;
  return -1; // Invalid hex character
}

static const char *fGetfield(char *line, int index)
{
  const char *token;
  for (token = strtok(line, "\t"); token != NULL; token = strtok(NULL, "\t\n"))
  {
    if (index == 0)
    {
      return token;
    }
    index--;
  }
  return NULL;
}

static int fResolve(int inUnicode, int **resolveTable)
{
  int i = 0;
  while (resolveTable[i] != NULL)
  {
    // Skip control characters, return -2
    if (inUnicode == ConstructUnicodeChar("U+00200E")) // LTR Mark
    {
      return -2;
    }
    if (resolveTable[i][0] == inUnicode)
    {
      return resolveTable[i][1];
    }
    i++;
  }
  return -1;
}

// Free the memory yourself
char *NormalizeVietnamese(const char *str, int **resolveTable)
{
  // Vietnamese characters range (ref: https://vietunicode.sourceforge.net/charset/)
  // U+0000 -> U+007F (1 byte each) (Basic Latin/ASCII)
  // U+00C0 -> U+01B0 (2 bytes each)
  // U+1EA0 -> U+1EF9 (3 bytes each)
  // vietnam dong: U+20AB (3 bytes)
  // Remember to free the memory
  char *res = MemAlloc(strlen(str) + 1);
  int i = 0;
  unsigned char *ptr = (unsigned char *)str;

  while (ptr && *ptr && i < strlen(str))
  {
    // 0...
    if ((*ptr & 0x80) == 0)
    {
      res[i++] = *ptr;
      ptr++;
    }
    // 110...
    // E0 = 1110 0000 mask
    // C0 = 1100 0000
    else if ((*ptr & 0xE0) == 0xC0)
    {
      // 2-byte character (UTF-8)
      int c = (*ptr << 8) | *(ptr + 1);
      int resolve = fResolve(c, resolveTable);
      if (resolve == -1)
      {
        res[i++] = c;
      }
      else if (resolve >= 0)
      {
        res[i++] = resolve;
      }
      ptr += 2;
    }
    // 1110...
    // F0 = 1111 0000 mask
    // E0 = 1110
    else if ((*ptr & 0xF0) == 0xE0)
    {
      // 3-byte character (UTF-8)
      int c = (*ptr << 16) | (*(ptr + 1) << 8) | *(ptr + 2);
      int resolve = fResolve(c, resolveTable);
      if (resolve == -1)
      {
        res[i++] = c;
      }
      else if (resolve >= 0)
      {
        res[i++] = resolve;
      }
      ptr += 3;
    }
    else
    {
      // 4-byte character (UTF-8)
      int c = (*ptr << 24) | (*(ptr + 1) << 16) | (*(ptr + 2) << 8) | *(ptr + 3);
      int resolve = fResolve(c, resolveTable);
      if (resolve == -1)
      {
        res[i++] = c;
      }
      else if (resolve >= 0)
      {
        res[i++] = resolve;
      }
      ptr += 4;
    }
  }

  res[i] = '\0';

  return res;
}

// U+uvwxyz
// ref: https://en.wikipedia.org/wiki/UTF-8#Description
int ConstructUnicodeChar(const char *codePoint)
{
  int uc = 0;
  int u = fHexToDec(codePoint[2]);
  int v = fHexToDec(codePoint[3]);
  int w = fHexToDec(codePoint[4]);
  int x = fHexToDec(codePoint[5]);
  int y = fHexToDec(codePoint[6]);
  int z = fHexToDec(codePoint[7]);
  if (strcmp(codePoint, "U+000080") < 0)
  {
    // 0yyyzzzz
    uc = (y << 4) | z;
    uc &= 0x7F;
    return uc;
  }
  else if (strcmp(codePoint, "U+000800") < 0)
  {
    // 110xxxyy | 10yyzzzz
    uc = (6 << 5) | ((x & 7) << 2) | (y >> 2);
    uc <<= 8;
    uc |= (2 << 6) | ((y & 3) << 4) | z;
    return uc;
  }
  else if (strcmp(codePoint, "U+010000") < 0)
  {
    // 1110wwww |	10xxxxyy |	10yyzzzz
    uc = (0xE << 4) | w;
    uc <<= 8;
    uc |= (2 << 6) | ((x & 0xF) << 2) | (y >> 2);
    uc <<= 8;
    uc |= (2 << 6) | ((y & 3) << 4) | z;
    return uc;
  }
  else
  {
    // 11110uvv	| 10vvwwww |	10xxxxyy |	10yyzzzz
    uc = (0x1F << 3) | ((u & 1) << 1) | (v >> 2);
    uc <<= 8;
    uc |= (2 << 6) | ((v & 3) << 4) | w;
    uc <<= 8;
    uc |= (2 << 6) | (x << 2) | (y >> 2);
    uc <<= 8;
    uc |= (2 << 6) | ((y & 3) << 4) | z;
  }
}

int **ReadResolveTable(const char *path)
{
  FILE *file = fopen(path, "r");
  if (!file){
    return NULL; // Return NULL if the file cannot be opened
  }

  // Dynamically allocate memory for the table (2D array of strings)
  int **table = (int **)MemAlloc(256 * sizeof(int *));
  for (int i = 0; i < 256; i++)
  {
    table[i] = (int *)MemAlloc(2 * sizeof(int));
  }

  char line[1024];
  const int unicodeHex = 1;
  const int vnQR = 10;
  int k = 0;
  bool firstLine = true;
  while (fgets(line, 1024, file))
  {
    if (firstLine)
    {
      firstLine = false;
      continue; // Skip the headers
    }
    // Extract fields using fGetfield (you may need to implement this function)
    char *tmp1 = strdup(line);
    const char *unicode = fGetfield(tmp1, unicodeHex);
    char *tmp2 = strdup(line);
    const char *qr = fGetfield(tmp2, vnQR);  
    table[k][0] = ConstructUnicodeChar(unicode);
    table[k][1] = qr[0];
    // Clean up temporary strings
    MemFree(tmp1);
    MemFree(tmp2);
    k++;
  }

  fclose(file);
  table[k] = NULL; // NULL-terminate the array of pointers
  
  return table;
}
