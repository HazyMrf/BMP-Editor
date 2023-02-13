#include "Image.h"
#include <fstream>


Image::Image(const std::string &file_name) : file_name_(file_name) {
    Read(file_name);
}

void Image::Read(const std::string &input_file) {
    std::ifstream input;
    input.open(input_file, std::ios::binary);

    if (!input.is_open()) {
        throw std::runtime_error("Cannot open input file\n");
    }

    input.read(reinterpret_cast<char *>(&headers_info_), sizeof(headers_info_));

    if (headers_info_.file_type != 0x4D42) {
        input.close();
        throw std::runtime_error("The only supported file format is BMP\n");
    }

    if (headers_info_.bits_per_pixel != 24) {
        input.close();
        throw std::runtime_error("The number of bits per pixel has to be 24\n");
    }

    if (headers_info_.DIBHeader_size != 40) {
        input.close();
        throw std::runtime_error("DIB header size must be 40 bits. Check your file format");
    }

    uint32_t length = headers_info_.width_ * headers_info_.bits_per_pixel / 8;
    uint8_t padding_amount = (4 - length % 4) % 4;
    unsigned char color[3];

    pixel_storage_.resize(headers_info_.height_);

    for (size_t i = 0; i < headers_info_.height_; ++i) {
        for (size_t j = 0; j < headers_info_.width_; ++j) {
            input.read(reinterpret_cast<char *>(color), 3);
            pixel_storage_[i].push_back(Pixel{color[0], color[1], color[2]});
        }
        input.ignore(padding_amount);
    }
    input.close();
}

void Image::Write(const std::string &output_file) {
    std::ofstream output;
    output.open(output_file, std::ios::binary);

    if (!output.is_open()) {
        throw std::runtime_error("Cannot open output file");
    }

    output.write(reinterpret_cast<char *>(&headers_info_), sizeof(headers_info_));

    uint32_t length = headers_info_.width_ * headers_info_.bits_per_pixel / 8;
    uint8_t padding_amount = (4 - length % 4) % 4;
    unsigned char padding_chars[] = {0, 0, 0};

    for (size_t index = 0; index < headers_info_.height_; ++index) {
        output.write(reinterpret_cast<char *>(pixel_storage_[index].data()), length);
        output.write(reinterpret_cast<char *>(padding_chars), padding_amount);
    }
    output.close();
}