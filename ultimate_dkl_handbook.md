# The Ultimate DKL handbook

(c) jrf-g. All text in this documentation/book is licensed under [CC BY-NC 4.0](https://creativecommons.org/licenses/by-nc/4.0/). code snippets under GPL 3.0 unless noted otherwise
## Chapter 1: Before You Get Started
### Chapter 1.1: DKL vs SKL
SKLs are static
DKLs are dynamic
SKLs require recompilation of the kernel
DKLs dont require recompilation of anything except the usedkl module (Which is an SKL, by the way)
SKLs work on any version
DKLs are newer
SKLs are unsecure
DKLs are secure
## Chapter 2: The Standards
Lets take a look at the standards of DKLs!
### Chapter 2.1: Security Standards
all dkls must have an ImportKey in the header (security flag). by default it should be 0x0 and be in a macro called IMP_KEY.
they also must have a ImportKey verifier that checks if their ImportKey matches the GB_IMP_KEY macro in the verdkl header.
and they must refuse to run if the verifier did not run or returned false. here is what verification code would look like:
```c
// THIS CODE LICENSED UNDER THE JTECH OSS LICENSE 1.0
#include "verdkl.h"
#include <stdint.h>
#define IMP_KEY 0x0
uint32_t verify() {
  if (IMP_KEY == GB_IMP_KEY) {
    return 1;
  }
  return 0;
}
```
