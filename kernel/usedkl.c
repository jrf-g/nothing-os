// dkl modules are the NothingOS version of a DLL
// add dkl imports below
#include "msgdkl.h"
// just add the dkl call in the initdkl function
// the functions of all dkls should start with the name of the dkl
void initdkl() {
  msgdkl_print();
}
