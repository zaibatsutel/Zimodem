#include <Arduino.h>
#include "str.h"

void setCharArray(char **target, const char *src)
{
  if(src == NULL)
    return;
  if(*target != NULL)
    free(*target);
  *target = (char *)malloc(strlen(src)+1);
  strcpy(*target,src);
}

void freeCharArray(char **arr)
{
  if(*arr == NULL)
    return;
  free(*arr);
  *arr = NULL;
}

int modifierCompare(const char *match1, const char *match2)
{
  if(strlen(match1) != strlen(match2))
    return -1;
    
  for(unsigned int i1=0;i1<strlen(match1);i1++)
  {
    char c1=tolower(match1[i1]);
    bool found=false;
    for(unsigned int i2=0;i2<strlen(match2);i2++)
    {
      char c2=tolower(match2[i2]);
      found = found || (c1==c2);
    }
    if(!found)
      return -1;
  }
  return 0;
}
