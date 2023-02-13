#include "Filter.h"
#include <cmath>
#include <random>
#include <stdexcept>

std::vector<std::vector<Pixel>> WrapMatrix(const std::vector<std::vector<Pixel>> &pixel_matrix) {
    std::vector<std::vector<Pixel>> matrix_wrap;

    matrix_wrap.resize(pixel_matrix.size() + 2);

    matrix_wrap[0].push_back(Pixel{0, 0, 0});
    for (const auto &pixel: pixel_matrix[0]) {
        matrix_wrap[0].push_back(pixel);
    }
    matrix_wrap[0].push_back(Pixel{0, 0, 0});

    for (size_t i = 0; i < pixel_matrix.size(); ++i) {
        matrix_wrap[i + 1].push_back(pixel_matrix[i][0]);
        for (const auto &pixel: pixel_matrix[i]) {
            matrix_wrap[i + 1].push_back(pixel);
        }
        matrix_wrap[i + 1].push_back(pixel_matrix[i][pixel_matrix[0].size() - 1]);
    }

    matrix_wrap[pixel_matrix.size() + 1].push_back(Pixel{0, 0, 0});
    for (const auto &pixel: pixel_matrix[pixel_matrix.size() - 1]) {
        matrix_wrap[pixel_matrix.size() + 1].push_back(pixel);
    }
    matrix_wrap[pixel_matrix.size() + 1].push_back(Pixel{0, 0, 0});

    return matrix_wrap;
}

void Apply3x3Matrix(Image &image, const std::vector<std::vector<double>> &matrix) { // no need for NxN matrices
    auto matrix_wrap = WrapMatrix(image.pixel_storage_);

    for (size_t i = 0; i < image.headers_info_.height_; ++i) {
        for (size_t j = 0; j < image.headers_info_.width_; ++j) {
            double color_red = 0, color_green = 0, color_blue = 0;

            for (size_t fi = 0; fi < 3; ++fi) {
                for (size_t fj = 0; fj < 3; ++fj) {
                    color_red += matrix[fi][fj] * matrix_wrap[i + fi][j + fj].red;
                    color_green += matrix[fi][fj] * matrix_wrap[i + fi][j + fj].green;
                    color_blue += matrix[fi][fj] * matrix_wrap[i + fi][j + fj].blue;
                }
            }

            uint8_t pixel_red = static_cast<uint8_t>(std::min(255.0, std::max(0.0, color_red)));
            uint8_t pixel_green = static_cast<uint8_t>(std::min(255.0, std::max(0.0, color_green)));
            uint8_t pixel_blue = static_cast<uint8_t>(std::min(255.0, std::max(0.0, color_blue)));

            image.pixel_storage_[i][j] = Pixel{pixel_red, pixel_green, pixel_blue};
        }
    }
}

// Basic Filters

Crop::Crop(size_t width, size_t height) : width_(width), height_(height) {}

void Crop::Apply(Image &image) {
    if (image.headers_info_.height_ > height_) {
        image.headers_info_.height_ = height_;
    }
    if (image.headers_info_.width_ > width_) {
        image.headers_info_.width_ = width_;
    }
    std::reverse(image.pixel_storage_.begin(), image.pixel_storage_.end());
    image.pixel_storage_.resize(image.headers_info_.height_);
    std::reverse(image.pixel_storage_.begin(), image.pixel_storage_.end());
}

Grayscale::Grayscale() {}

void Grayscale::Apply(Image &image) {
    for (size_t i = 0; i < image.headers_info_.height_; ++i) {
        for (size_t j = 0; j < image.headers_info_.width_; ++j) {
            Pixel temp_pixel = image.pixel_storage_[i][j];
            image.pixel_storage_[i][j].red =
                    0.299 * temp_pixel.red + 0.587 * temp_pixel.green + 0.114 * temp_pixel.blue;
            image.pixel_storage_[i][j].green =
                    0.299 * temp_pixel.red + 0.587 * temp_pixel.green + 0.114 * temp_pixel.blue;
            image.pixel_storage_[i][j].blue =
                    0.299 * temp_pixel.red + 0.587 * temp_pixel.green + 0.114 * temp_pixel.blue;
        }
    }
}

Negative::Negative() {}

void Negative::Apply(Image &image) {
    for (size_t i = 0; i < image.headers_info_.height_; ++i) {
        for (size_t j = 0; j < image.headers_info_.width_; ++j) {
            Pixel temp_pixel = image.pixel_storage_[i][j];
            image.pixel_storage_[i][j].red = 255 - temp_pixel.red;
            image.pixel_storage_[i][j].green = 255 - temp_pixel.green;
            image.pixel_storage_[i][j].blue = 255 - temp_pixel.blue;
        }
    }
}

Sharpening::Sharpening() {}

void Sharpening::Apply(Image &image) {
    Apply3x3Matrix(image, {{0,  -1, 0},
                           {-1, 5,  -1},
                           {0,  -1, 0}});
}

EdgeDetection::EdgeDetection(float threshold) : threshold_(threshold) {}

void EdgeDetection::Apply(Image &image) {
    Apply3x3Matrix(image, {{0,  -1, 0},
                           {-1, 4,  -1},
                           {0,  -1, 0}});

    for (size_t i = 0; i < image.headers_info_.height_; ++i) {
        for (auto &pixel: image.pixel_storage_[i]) {
            if (0.299 * pixel.red + 0.567 * pixel.green + 0.114 * pixel.blue > 255.0 * threshold_) {
                pixel = Pixel{255, 255, 255};
            } else {
                pixel = Pixel{0, 0, 0};
            }
        }
    }
}

GaussianBlur::GaussianBlur(float sigma) : sigma_(sigma) {}

std::vector<size_t> GaussianBlur::BoxesForGauss(size_t n) {
    // n - number of "boxes"

    std::vector<size_t> sizes;

    size_t ideal_size = sqrt((12 * sigma_ * sigma_ / n) + 1); // length of sliding window
    size_t size_lower = floor(ideal_size);

    if (size_lower % 2 == 0) {
        --size_lower;
    }

    size_t size_upper = size_lower + 2;
    size_t mid_size =
            (12 * sigma_ * sigma_ - n * size_lower * size_lower - 4 * n * size_lower - 3 * n) / (-4 * size_lower - 4);
    size_t m = round(mid_size);

    for (size_t i = 0; i < n; ++i) {
        sizes.push_back(i < m ? size_lower : size_upper);
    }
    return sizes;
}

void
GaussianBlur::HorizontalBlur(const std::vector<std::vector<Pixel>> &source, std::vector<std::vector<Pixel>> &target,
                             size_t radius) {
    size_t height = source.size();
    size_t width = source[0].size();

    for (size_t i = 0; i < height; ++i) {

        double val_red = (radius + 1) * source[i][0].red;
        double val_green = (radius + 1) * source[i][0].green;
        double val_blue = (radius + 1) * source[i][0].blue;
        size_t left = 0, right = radius;

        for (size_t j = 0; j < radius; ++j) {
            val_red += source[i][j].red;
            val_green += source[i][j].green;
            val_blue += source[i][j].blue;
        }

        for (size_t j = 0; j < width; ++j) {
            if (j <= radius) {
                val_red += source[i][right].red - source[i][0].red;
                val_green += source[i][right].green - source[i][0].green;
                val_blue += source[i][right].blue - source[i][0].blue;
                ++right;
            } else if (j < width - radius) {
                val_red += source[i][right].red - source[i][left].red;
                val_green += source[i][right].green - source[i][left].green;
                val_blue += source[i][right].blue - source[i][left].blue;
                ++right;
                ++left;
            } else {
                val_red += source[i][width - 1].red - source[i][left].red;
                val_green += source[i][width - 1].green - source[i][left].green;
                val_blue += source[i][width - 1].blue - source[i][left].blue;
                ++left;
            }
            target[i][j].red = round(val_red / (2 * radius + 1));
            target[i][j].green = round(val_green / (2 * radius + 1));
            target[i][j].blue = round(val_blue / (2 * radius + 1));
        }
    }
}

void GaussianBlur::VerticalBlur(const std::vector<std::vector<Pixel>> &source, std::vector<std::vector<Pixel>> &target,
                                size_t radius) {
    size_t height = source.size();
    size_t width = source[0].size();

    for (size_t j = 0; j < width; ++j) {

        double val_red = (radius + 1) * source[0][j].red;
        double val_green = (radius + 1) * source[0][j].green;
        double val_blue = (radius + 1) * source[0][j].blue;
        size_t left = 0, right = radius;

        for (size_t i = 0; i < radius; ++i) {
            val_red += source[i][j].red;
            val_green += source[i][j].green;
            val_blue += source[i][j].blue;
        }

        for (size_t i = 0; i < height; ++i) {
            if (i <= radius) {
                val_red += source[right][j].red - source[0][j].red;
                val_green += source[right][j].green - source[0][j].green;
                val_blue += source[right][j].blue - source[0][j].blue;
                ++right;
            } else if (i < width - radius) {
                val_red += source[right][j].red - source[left][j].red;
                val_green += source[right][j].green - source[left][j].green;
                val_blue += source[right][j].blue - source[left][j].blue;
                ++right;
                ++left;
            } else {
                val_red += source[height - 1][j].red - source[left][j].red;
                val_green += source[height - 1][j].green - source[left][j].green;
                val_blue += source[height - 1][j].blue - source[left][j].blue;
                ++left;
            }
            target[i][j].red = round(val_red / (2 * radius + 1));
            target[i][j].green = round(val_green / (2 * radius + 1));
            target[i][j].blue = round(val_blue / (2 * radius + 1));
        }
    }
}

void
GaussianBlur::BoxBlur(std::vector<std::vector<Pixel>> &source, std::vector<std::vector<Pixel>> &target, size_t radius) {
    target = source; // making target and source equally changed after previous BoxBlur funcs applied
    HorizontalBlur(target, source, radius); // changing source using target as a copy
    VerticalBlur(source, target, radius); // changing target using source
}

void GaussianBlur::Apply(Image &image) {
    std::vector<size_t> boxes = BoxesForGauss(4); // can be > 4, but the result is almost the same
    auto storage_copy = image.pixel_storage_;
    BoxBlur(image.pixel_storage_, storage_copy, (boxes[0] - 1) / 2);
    BoxBlur(storage_copy, image.pixel_storage_, (boxes[1] - 1) / 2);
    BoxBlur(image.pixel_storage_, storage_copy, (boxes[2] - 1) / 2);
    BoxBlur(storage_copy, image.pixel_storage_, (boxes[3] - 1) / 2);

}

// Extra Filters

AutoContrast::AutoContrast() {}

void AutoContrast::ContrastNumber(uint8_t &number) {
    if (number >= 100 && number <= 235) {
        number += 20;
    } else if (number > 235) {
        number = 255;
    } else if (number <= 40 && number >= 20) {
        number -= 20;
    } else if (number < 20) {
        number = 0;
    } else if (number > 70 && number < 100) {
        number += 10;
    } else if (number < 60 && number > 40) {
        number -= 10;
    }
}

void AutoContrast::Apply(Image &image) {
    for (size_t i = 0; i < image.headers_info_.height_; ++i) {
        for (size_t j = 0; j < image.headers_info_.width_; ++j) {
            ContrastNumber(image.pixel_storage_[i][j].red);
            ContrastNumber(image.pixel_storage_[i][j].green);
            ContrastNumber(image.pixel_storage_[i][j].blue);
        }
    }
}

Gamma::Gamma(float sigma) : sigma_(sigma) {}

void Gamma::Apply(Image &image) {
    for (size_t i = 0; i < image.headers_info_.height_; ++i) {
        for (size_t j = 0; j < image.headers_info_.width_; ++j) {

            image.pixel_storage_[i][j].red = pow(image.pixel_storage_[i][j].red, sigma_);
            image.pixel_storage_[i][j].green = pow(image.pixel_storage_[i][j].green, sigma_);
            image.pixel_storage_[i][j].blue = pow(image.pixel_storage_[i][j].blue, sigma_);

        }
    }
}

PixelImage::PixelImage(size_t pixel_size) : pixel_size_(pixel_size) {}

void PixelImage::Apply(Image &image) {
    if (pixel_size_ > image.headers_info_.height_ || pixel_size_ > image.headers_info_.height_) {
        throw std::runtime_error("Pixel size must be less than image's height and width\n");
    }

    for (size_t i = pixel_size_; i < image.headers_info_.height_ - pixel_size_; i += 2 * pixel_size_ + 1) {
        for (size_t j = pixel_size_; j < image.headers_info_.width_ - pixel_size_; j += 2 * pixel_size_ + 1) {
            size_t var_red = 0;
            size_t var_green = 0;
            size_t var_blue = 0;
            for (int fi = i - pixel_size_; fi <= i + pixel_size_; ++fi) {
                for (int fj = j - pixel_size_; fj <= j + pixel_size_; ++fj) {
                    var_red += image.pixel_storage_[fi][fj].red;
                    var_green += image.pixel_storage_[fi][fj].green;
                    var_blue += image.pixel_storage_[fi][fj].blue;
                }
            }
            if (pixel_size_ != 0) {
                var_red /= (2 * pixel_size_ + 1) * (2 * pixel_size_ + 1);
                var_green /= (2 * pixel_size_ + 1) * (2 * pixel_size_ + 1);
                var_blue /= (2 * pixel_size_ + 1) * (2 * pixel_size_ + 1);
            }
            for (int fi = i - pixel_size_; fi <= i + pixel_size_; ++fi) {
                for (int fj = j - pixel_size_; fj <= j + pixel_size_; ++fj) {
                    image.pixel_storage_[fi][fj].red = var_red;
                    image.pixel_storage_[fi][fj].green = var_green;
                    image.pixel_storage_[fi][fj].blue = var_blue;
                }
            }
        }
    }
}

Crystallization::Crystallization(size_t shard_size) : shard_size_(shard_size) {}

Crystallization::SquareCenter Crystallization::FindCentralSquares(size_t height, size_t width) {
    SquareCenter squares_centers;
    std::random_device rd;
    std::mt19937 gen(rd()); // randomizer
    std::uniform_int_distribution<> dis(0, shard_size_ - 1);
    for (size_t i = 0; i < height; i += shard_size_) {
        for (size_t j = 0; j < width; j += shard_size_) {
            size_t x = dis(gen);
            size_t y = dis(gen);
            squares_centers[i / shard_size_][j / shard_size_] = {i + x, j + y};
            centers_.push_back({i + x, j + y});
        }
    }
    return squares_centers;
}

Crystallization::Shards Crystallization::FormShards(size_t height, size_t width) {
    SquareCenter square_centers = FindCentralSquares(height, width);
    Shards blocks;
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            size_t min_distance = UINT64_MAX;
            std::pair<size_t, size_t> center;
            size_t sqr_x = i / shard_size_;
            size_t sqr_y = j / shard_size_;
            for (int fi = -1; fi <= 1; ++fi) {
                for (int fj = -1; fj <= 1; ++fj) {
                    if (sqr_x + fi < height && sqr_y + fj < width &&
                        sqr_x + fi >= 0 && sqr_y + fj >= 0) {
                        size_t center_x = square_centers[sqr_x + fi][sqr_y + fj].first;
                        size_t center_y = square_centers[sqr_x + fi][sqr_y + fj].second;
                        size_t distance = (center_x - i) * (center_x - i) + (center_y - j) * (center_y - j);
                        if (distance < min_distance) {
                            min_distance = distance;
                            center = {center_x, center_y};
                        }
                    }
                }
            }
            blocks[center.first][center.second].push_back({i, j});
        }
    }
    return blocks;
}

void Crystallization::Apply(Image &image) {
    Shards blocks = FormShards(image.headers_info_.height_, image.headers_info_.width_);
    for (const auto&[center_x, center_y]: centers_) {
        size_t var_red = 0;
        size_t var_green = 0;
        size_t var_blue = 0;
        for (const auto&[point_x, point_y]: blocks[center_x][center_y]) {
            var_red += image.pixel_storage_[point_x][point_y].red;
            var_green += image.pixel_storage_[point_x][point_y].green;
            var_blue += image.pixel_storage_[point_x][point_y].blue;
        }
        if (!blocks[center_x][center_y].empty()) {
            var_red /= blocks[center_x][center_y].size();
            var_green /= blocks[center_x][center_y].size();
            var_blue /= blocks[center_x][center_y].size();
        }

        for (const auto&[point_x, point_y]: blocks[center_x][center_y]) {
            image.pixel_storage_[point_x][point_y].red = var_red;
            image.pixel_storage_[point_x][point_y].green = var_green;
            image.pixel_storage_[point_x][point_y].blue = var_blue;
        }
    }
}
