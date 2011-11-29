#include <iostream>

int main() {
  uint32_t start = 0;
  uint32_t end = 600;
  uint32_t fade_in = 200;
  uint32_t fade_out = 200;
  for (uint32_t i = 0; i <= 600; i++) {
    if (start <= i && i <= start+fade_in) {
      effect_volume *= 1.0/fade_in*(i-start);
    }
    if (end-fade_out <= i && i <= end) {
      effect_volume *= 1.0-(1.0/fade_out*(i-(end-fade_out)));
    }
  }
}