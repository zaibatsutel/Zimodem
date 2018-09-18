#ifndef __STR_H__
#define __STR_H__

void setCharArray(char **target, const char *src);
void freeCharArray(char **arr);
int modifierCompare(const char *match1, const char *match2);

inline char lc(char c)
{
  if((c>=65) && (c<=90))
    return c+32;
  if((c>=193) && (c<=218))
    return c-96;
  return c;
}
#endif
