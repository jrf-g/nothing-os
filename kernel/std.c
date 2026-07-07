typedef _Bool bool;
int strcmp(const char *a, const char *b) {
  while (*a && (*a == *b)) {
      a++;
      b++;
  }
  return *(unsigned char*)a - *(unsigned char*)b;
}
int strequ(const char *aequ, const char *bequ) {
  int buffa = strcmp(aequ, bequ);
  if (buffa = 0) {
    result = 1;
  } else {
    result = 0;
  }
  return result;
}
