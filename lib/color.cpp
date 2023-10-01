
#include "color.h"

/*void write_color(const std::string& outputFile, const std::vector<std::vector<color>>& frameBuffer, const int samples_per_pixel) {
    
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    auto scale = 1.0 / samples_per_pixel;
    r = std::sqrt(r * scale); 
    g = std::sqrt(g * scale);
    b = std::sqrt(b * scale);

    os << static_cast<int>(255.999 * clamp(r, 0.0, 0.999)) << ' '
      << static_cast<int>(255.999 * clamp(g, 0.0, 0.999)) << ' '
      << static_cast<int>(255.999 * clamp(b, 0.0, 0.999)) << '\n';
}*/