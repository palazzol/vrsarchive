#ifdef BSD
#define memcpy(to, from, n) bcopy(from, to, n)
#endif
