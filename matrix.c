// keyboards/yourkbd/matrix.c
#include "quantum.h"
#include "wait.h"
#include <string.h>

// ============================ 物理ピン定義 ============================
// 行4本（上→下）
static const pin_t ROW_PINS[4] = { GP5, GP6, GP7, GP8 };

// 第1パス（ROW2COL）で使う列 8 本（右：16..9）
static const pin_t COL_PINS_PASS1[8] = { GP16, GP15, GP14, GP13, GP12, GP11, GP10, GP9 };

// 第2パス（COL2ROW）で使う列 4 本（左：12..9）
static const pin_t COL_PINS_PASS2[4] = { GP12, GP11, GP10, GP9 };

// 論理列の割当：第1パス=0..7, 第2パス=8..11
#define COL_OFFSET_PASS1 0
#define COL_OFFSET_PASS2 8

// ============================ ユーティリティ ============================
static inline void as_input_pullup(pin_t p){ if(p!=NO_PIN){ gpio_set_pin_input_high(p);} }
static inline void drive_low(pin_t p){ if(p!=NO_PIN){ gpio_set_pin_output(p); gpio_write_pin_low(p);} }
static inline void hi_z(pin_t p){ if(p!=NO_PIN){ gpio_set_pin_input_high(p);} }
static inline bool is_low(pin_t p){ return (p!=NO_PIN) ? (gpio_read_pin(p) == 0) : false; }

// 変更検出用
static matrix_row_t last_matrix[MATRIX_ROWS];

void matrix_init_custom(void) {
    // すべて Hi-Z/入力プルアップに
    for (uint8_t r = 0; r < 4; r++) as_input_pullup(ROW_PINS[r]);
    for (uint8_t c = 0; c < 8; c++) as_input_pullup(COL_PINS_PASS1[c]);
    for (uint8_t c = 0; c < 4; c++) as_input_pullup(COL_PINS_PASS2[c]);
    memset(last_matrix, 0, sizeof(last_matrix));
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    // 初期クリア
    for (uint8_t r = 0; r < MATRIX_ROWS; r++) current_matrix[r] = 0;

    // ------------------------------------------------------------------
    // Pass 1: ROW2COL（列を選択 → 行を読む） → 論理列 0..7
    // ------------------------------------------------------------------
    for (uint8_t r = 0; r < 4; r++) as_input_pullup(ROW_PINS[r]);
    for (uint8_t c = 0; c < 8; c++) hi_z(COL_PINS_PASS1[c]);

    for (uint8_t c = 0; c < 8; c++) {
        drive_low(COL_PINS_PASS1[c]);     // 列選択（Low）
        wait_us(5);                       // 安定待ち（QMK 既定実装相当の微少ディレイ）  // [3](https://docs.qmk.fm/custom_matrix)
        for (uint8_t r = 0; r < 4; r++) {
            if (is_low(ROW_PINS[r])) {
                current_matrix[r] |= (matrix_row_t)1 << (COL_OFFSET_PASS1 + c);
            }
        }
        hi_z(COL_PINS_PASS1[c]);          // 列解除
    }

    // ------------------------------------------------------------------
    // Pass 2: COL2ROW（行を選択 → 列を読む） → 論理列 8..11（左4列の“相方キー”）
    // ------------------------------------------------------------------
    for (uint8_t c = 0; c < 4; c++) as_input_pullup(COL_PINS_PASS2[c]);
    for (uint8_t r = 0; r < 4; r++) hi_z(ROW_PINS[r]);

    for (uint8_t r = 0; r < 4; r++) {
        drive_low(ROW_PINS[r]);           // 行選択（Low）
        wait_us(5);                       // 安定待ち                                           // [2](https://joshinator.de/posts/qmk-gamepad-part-3/)
        for (uint8_t c = 0; c < 4; c++) {
            if (is_low(COL_PINS_PASS2[c])) {
                current_matrix[r] |= (matrix_row_t)1 << (COL_OFFSET_PASS2 + c);
            }
        }
        hi_z(ROW_PINS[r]);                // 行解除
    }

    // 変更検出（任意最適化）
    bool changed = memcmp(last_matrix, current_matrix, sizeof(last_matrix)) != 0;
    if (changed) memcpy(last_matrix, current_matrix, sizeof(last_matrix));
    return changed;
}
