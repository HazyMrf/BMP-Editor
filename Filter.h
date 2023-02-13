#pragma once

#include "ImageParser.h"
#include "Image.h"
#include <unordered_map>

class Filter {
public:
    virtual void Apply(Image &image) = 0;

    virtual ~Filter() = default;
};

// Basic Filters

class Crop : public Filter {
public:
    Crop(size_t width, size_t height);

    void Apply(Image &image) override;

private:
    size_t width_;
    size_t height_;
};

class Grayscale : public Filter {
public:
    Grayscale();

    void Apply(Image &image) override;
};

class Negative : public Filter {
public:
    Negative();

    void Apply(Image &image) override;
};

class Sharpening : public Filter {
public:
    Sharpening();

    void Apply(Image &image) override;
};

class EdgeDetection : public Filter {
public:
    EdgeDetection(float threshold);

    void Apply(Image &image) override;

private:
    float threshold_;
};

class GaussianBlur : public Filter {
    // Complexity of this filter is O(height * width) for any sigma
public:
    GaussianBlur(float sigma);

    std::vector<size_t> BoxesForGauss(size_t n);

    void
    HorizontalBlur(const std::vector<std::vector<Pixel>> &source, std::vector<std::vector<Pixel>> &target,
                   size_t radius);

    void
    VerticalBlur(const std::vector<std::vector<Pixel>> &source, std::vector<std::vector<Pixel>> &target, size_t radius);

    void BoxBlur(std::vector<std::vector<Pixel>> &source, std::vector<std::vector<Pixel>> &target, size_t radius);

    void Apply(Image &image) override;

private:
    float sigma_;
};

// Extra Filters

class AutoContrast : public Filter { // speaks for himself
public:
    AutoContrast();

    void ContrastNumber(uint8_t& number);

    void Apply(Image &image) override;
};

class Gamma : public Filter { // just gamma filter, controls the brightness of picture
public:
    Gamma(float sigma);

    void Apply(Image &image) override;

private:
    float sigma_;
};

class PixelImage : public Filter { // makes an image made of pixels bigger size (pixel blurring)
public:
    PixelImage(size_t pixel_size);

    void Apply(Image &image) override;

private:
    size_t pixel_size_;
};

class Crystallization : public Filter { // crystallize an image
public:

    using SquareCenter = std::unordered_map<size_t, std::unordered_map<size_t, std::pair<size_t, size_t>>>;
    using Shards = std::unordered_map<size_t, std::unordered_map<size_t, std::vector<std::pair<size_t, size_t>>>>;

    Crystallization(size_t shard_size);

    SquareCenter FindCentralSquares(size_t height, size_t width);

    Shards FormShards(size_t height, size_t width);

    void Apply(Image &image) override;

private:
    size_t shard_size_;
    std::vector<std::pair<size_t, size_t>> centers_;
};

