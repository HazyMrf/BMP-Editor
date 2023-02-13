#include <inttypes.h>

#pragma pack(push, 1)
struct BitmapFileHeader {
    uint16_t file_type;
    uint32_t file_size;
    uint16_t reserved_size1;
    uint16_t reserved_size2;
    uint32_t offset;
};


struct DIBHeader {
    uint32_t DIBHeader_size;
    uint32_t width_;
    uint32_t height_;
    uint16_t color_planes;
    uint16_t bits_per_pixel;
    uint32_t compression_method;
    uint32_t image_size;
    uint32_t horizontal_resolution;
    uint32_t vertical_resolution;
    uint32_t colors_number;
    uint32_t important_colors_number;
};

struct BMPHeaders : BitmapFileHeader, DIBHeader {

};
#pragma pack(pop)

struct Pixel {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};
