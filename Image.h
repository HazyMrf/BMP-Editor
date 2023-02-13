#include "BMPstruct.h"
#include <vector>
#include <string>

class Image {
public:
    friend void Apply3x3Matrix(Image &image, const std::vector<std::vector<double>>& matrix);

    friend class Crop;

    friend class Grayscale;

    friend class Negative;

    friend class Sharpening;

    friend class EdgeDetection;

    friend class GaussianBlur;

    friend class AutoContrast;

    friend class Gamma;

    friend class PixelImage;

    friend class Crystallization;

    Image(const std::string &file_name);

    void Read(const std::string &input_file);

    void Write(const std::string &output_file);

private:
    std::string file_name_; // optional parameter, never to be used
    BMPHeaders headers_info_;
    std::vector<std::vector<Pixel>> pixel_storage_;
};
