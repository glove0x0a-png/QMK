// keyboards/yourkbd/keymaps/default/keymap.c
#include QMK_KEYBOARD_H

// 4行×12列
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
[0] = {
  // ---- row 0 ----
  {KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_MINS, KC_ENT, KC_LSFT, KC_CAPS, KC_TAB},
  // ---- row 1 ----
  {KC_F, KC_G, KC_H, KC_NO, KC_K, KC_L, KC_SCLN, KC_ENT, KC_LGUI, KC_Z, KC_A, KC_Q},
  // ---- row 2 ----
  {KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_UP, KC_BSLS, KC_LALT, KC_X, KC_S, KC_W},
  // ---- row 3 ----
  {MO(1), KC_SPC, KC_NO, MO(1), KC_NO, KC_LEFT, KC_DOWN, KC_RGHT, KC_LCTL, KC_C, KC_D, KC_E}
}
};
