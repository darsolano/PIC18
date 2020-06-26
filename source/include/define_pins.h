/*
 * define_pins.h
 *
 *  Created on: Apr 1, 2016
 *      Author: dsolano
 */

#ifndef INCLUDE_DEFINE_PINS_H_
#define INCLUDE_DEFINE_PINS_H_

/* define_pins.h
 *
 * Define a useful macro for giving GPIO pins nicer, semantic names.
 *
 * The only macro exported is DEFINE_PIN. Use as:
 *  DEFINE_PIN(PIN_NAME, port number, pin number)
 * this will define the following:
 *  PIN_NAME_DEASSERT(), _OFF(), _LOW() to bring the pin low
 *  PIN_NAME_ASSERT(), _ON(), _HIGH() to bring the pin high
 *  PIN_NAME_INPUT() to make the pin an input
 *  PIN_NAME_OUTPUT() to make the pin an output
 *  PIN_NAME_READ() to read the current state of the pin
 *                  (returns a 1 or 0, typed as a uint_fast8_t)
 */

#include <stdint.h>
#include <xc.h>
#include <types.h>


#define DIR_INPUT   1
#define DIR_OUTPUT  0


#define DEFINE_PIN(name, port, pin) \
inline static void name##_DEASSERT() { \
  LAT##port &= ~(1<<pin); \
} \
inline static void name##_OFF() { \
  LAT##port &= ~(1<<pin); \
} \
inline static void name##_LOW() { \
  LAT##port &= ~(1<<pin); \
} \
inline static void name##_ASSERT() { \
  LAT##port |= (1<<pin); \
} \
inline static void name##_ON() { \
  LAT##port |= (1<<pin); \
} \
inline static void name##_HIGH() { \
  LAT##port |= (1<<pin); \
} \
inline static void name##_TOGGLE() { \
  LAT##port ^= (1<<pin); \
} \
inline static void name##_AS_INPUT() { \
  TRIS##port |= (1<<pin); \
} \
inline static void name##_AS_OUTPUT() { \
  TRIS##port &= ~(1<<pin); \
} \
inline static uint_fast8_t name##_READ() { \
  return (PORT##port & (1<<pin)) >> pin;\
}


#endif /* INCLUDE_DEFINE_PINS_H_ */
