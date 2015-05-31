#ifndef __PIPARA_H__
#define __PIPARA_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int pipara_setup(void);

int pipara_write(int8_t data);

#ifdef __cplusplus
}
#endif

#endif

