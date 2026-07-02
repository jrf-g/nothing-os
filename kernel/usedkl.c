// dkl modules are the NothingOS version of a DLL
// add dkl inlcudes below
#include "msg.dkl"
// just add the dkl call in the initdkl function
// the functions of all dkls should start with the name of the dkl
void initdkl() {
  msgdkl_verify(); // activate msgdkl
  msgdkl_print(); // use msgdkl
}
