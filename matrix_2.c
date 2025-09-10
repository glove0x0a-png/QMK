#include "quantum.h"
#include "gpio.h"
#include <string.h>

// ピン定義
const pin_t raw2col_rows[] = { GP5, GP6, GP7, GP8 };
const pin_t raw2col_cols[] = { GP16, GP15, GP14, GP13, GP12, GP11, GP10, GP9 };

const pin_t col2raw_rows[] = { GP9, GP10, GP11, GP12 };
const pin_t col2raw_cols[] = { GP5, GP6, GP7, GP8 };

// マトリックスサイズ
#define MATRIX_ROWS 8
#define MATRIX_COLS 8

// マトリックスデータ
matrix_row_t matrix[MATRIX_ROWS];
matrix_row_t raw_matrix[MATRIX_ROWS];

// 初期化関数
void matrix_init_custom(void) {
    // 行ピン（出力 → HIGH）
    for (uint8_t i = 0; i < 4; i++) {
        if (raw2col_rows[i] != NO_PIN) {
            setPinOutput(raw2col_rows[i]);
            writePinHigh(raw2col_rows[i]);
        }
        if (col2raw_rows[i] != NO_PIN) {
            setPinOutput(col2raw_rows[i]);
            writePinHigh(col2raw_rows[i]);
        }
    }

    // 列ピン（入力 → pull-up）
    for (uint8_t i = 0; i < 8; i++) {
        if (raw2col_cols[i] != NO_PIN) {
            setPinInputHigh(raw2col_cols[i]);
        }
    }
    for (uint8_t i = 0; i < 4; i++) {
        if (col2raw_cols[i] != NO_PIN) {
            setPinInputHigh(col2raw_cols[i]);
        }
    }

    // マトリックス初期化
    memset(matrix, 0, sizeof(matrix));
    memset(raw_matrix, 0, sizeof(raw_matrix));
}

// スキャン関数
void key_scan_custom(void) {
    matrix_row_t curr_matrix[MATRIX_ROWS] = {0};

    // raw2col スキャン
    for (uint8_t row = 0; row < 4; row++) {
        pin_t row_pin = raw2col_rows[row];
        if (row_pin == NO_PIN) continue;

        setPinOutput(row_pin);
        writePinLow(row_pin);
        wait_us(30); // 安定化

        for (uint8_t col = 0; col < 8; col++) {
            pin_t col_pin = raw2col_cols[col];
            if (col_pin == NO_PIN) continue;

            setPinInputHigh(col_pin);
            bool pressed = !readPin(col_pin);
            if (pressed) {
                curr_matrix[row] |= (1 << col);
            }
        }

        writePinHigh(row_pin);
        setPinInputHigh(row_pin);
    }

    // col2raw スキャン
    for (uint8_t col = 0; col < 4; col++) {
        pin_t col_pin = col2raw_cols[col];
        if (col_pin == NO_PIN) continue;

        setPinOutput(col_pin);
        writePinLow(col_pin);
        wait_us(30);

        for (uint8_t row = 0; row < 4; row++) {
            pin_t row_pin = col2raw_rows[row];
            if (row_pin == NO_PIN) continue;

            setPinInputHigh(row_pin);
            bool pressed = !readPin(row_pin);
            if (pressed) {
                curr_matrix[row + 4] |= (1 << col);
            }
        }

        writePinHigh(col_pin);
        setPinInputHigh(col_pin);
    }

    // raw_matrix にコピー
    bool changed = memcmp(raw_matrix, curr_matrix, sizeof(curr_matrix)) != 0;
    if (changed) {
        memcpy(raw_matrix, curr_matrix, sizeof(curr_matrix));
    }

    // デバウンス処理
    debounce(raw_matrix, matrix, MATRIX_ROWS, changed);
}
