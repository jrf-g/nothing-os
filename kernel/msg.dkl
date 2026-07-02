#include "msgdkl.h"
#include "verdkl.h"
#include "powerctl.h"
#include "kprint.h"
int secure = 0;
void msgdkl_print() {
  if (!secure) {
    badlib();
  }
  kprint(DKLTXT);
}
void msgdkl_verify() {
  if (MSGDKL_IMP_KEY == GB_IMP_KEY) {
    int secure = 1;
  } else {
    int secure = 0;
  }
}
