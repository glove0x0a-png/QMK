// keyboards/yourkbd/config.h
#pragma once
#include "config_common.h"

// 論理マトリクス：4行 × 12列（第1パス 8列 + 第2パス 4列）
#define MATRIX_ROWS 4
#define MATRIX_COLS 12

// 表示上の既定。実際は両方向を custom で扱う
#define DIODE_DIRECTION ROW2COL

// 必要に応じて調整
#define DEBOUNCE 5
// #define MATRIX_IO_DELAY 30
