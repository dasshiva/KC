#ifndef __KC_H__
#define __KC_H__

enum Status {
    KC_SUCCESS = 0,
    KC_ERROR_OUT_OF_MEMORY,
    KC_UNKNOWN_ERROR,
    KC_IMPLEMENTATION_ERROR,
    KC_NO_SUITABLE_DEVICE

};

typedef void* KC_Handle;
int KC_Init(KC_Handle* handle);
void KC_Destroy(KC_Handle h);

#endif