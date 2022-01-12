

unsigned long ceil2e(unsigned long num){
  // 2**0 = 1
  if (num == 0 || num == 1)   return 1;
  unsigned long n = num - 1;
  int e = 1;
  while (n>>=1) ++e;
  unsigned long ret = 1;
  while (e--) ret<<=1;
  return ret;
}