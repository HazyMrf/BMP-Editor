#include "ImageParser.h"
#include <stdexcept>
#include <iostream>

bool IsAllDigits(const std::string &str) {
    return std::all_of(str.begin(), str.end(), ::isdigit);
}

bool IsFloat(const std::string &str) {
    char *ptr;
    strtof(str.data(), &ptr);
    return (*ptr) == '\0';
}

bool FilterInfo::operator==(const FilterInfo &other) const {
    return std::tie(name, params) == std::tie(other.name, other.params);
}

bool ParserResults::operator==(const ParserResults &other) const {
    return std::tie(input_file_path, output_file_path, filters) ==
           std::tie(other.input_file_path, other.output_file_path, other.filters);
}

ParserResults ImageParser::Parse(int argc, const char *argv[]) {
    if (argc == 1) {
        throw std::runtime_error(
                "You can choose filters from the following list:\n"
                "1.Crop (print -crop width height)\n"
                "2.Grayscale (print -gs)\n"
                "3.Negative (print -neg)\n"
                "4.Sharpening (print -sharp)\n"
                "5.Edge Detection (print -edge threshold)\n"
                "6.Gaussian Blur (print -blur sigma)\n"
                "7.Auto Contrast (print -contr)\n"
                "8.Gamma (print -gamma sigma)\n"
                "9.PixelImage (print -pixel pixel size)\n"
                "10.Crystallization (print -crystal shard size)\n"
                "Remember that you can use multiple filters at once\n");
    } else if (argc < 3) {
        throw std::runtime_error("You need to write input and output files\n");
    } else if (argc == 3) {
        std::cout << "If you want to work with your BMP image, write down some filters\n";
        return ParserResults{argv[1], argv[2], {}};
    } else {
        ParserResults results;
        results.input_file_path = argv[1];
        results.output_file_path = argv[2];
        if (argv[1] == argv[2]) {
            throw std::runtime_error("input and output file should be different files, so that input is unchanged\n");
        }
        int filter_index = -1;
        FilterInfo empty_filter{"", {}};
        for (size_t index = 3; index < argc; ++index) {
            if (argv[index][0] == '-') {
                ++filter_index;
                results.filters.push_back(empty_filter);
                results.filters[filter_index].name = argv[index];
            } else {
                if (filter_index == -1) {
                    throw std::runtime_error("You haven't passed the name of the filter\n");
                }
                results.filters[filter_index].params.push_back(argv[index]);
            }
        }

        for (const auto &filter : results.filters) {
            if (filter.name == "-crop") {
                if (filter.params.size() != 2) {
                    throw std::runtime_error("Crop filter has 2 parameters: width and height\n");
                } else if (!IsAllDigits(filter.params[0]) || !IsAllDigits(filter.params[1])) {
                    throw std::runtime_error("Crop filter parameters (width and height) must be integers\n");
                }
            } else if (filter.name == "-gs") {
                if (!filter.params.empty()) {
                    throw std::runtime_error("Grayscale filter doesn't have any parameters\n");
                }
            } else if (filter.name == "-neg") {
                if (!filter.params.empty()) {
                    throw std::runtime_error("Negative filter doesn't have any parameters\n");
                }
            } else if (filter.name == "-sharp") {
                if (!filter.params.empty()) {
                    throw std::runtime_error("Sharpening filter doesn't have any parameters\n");
                }
            } else if (filter.name == "-edge") {
                if (filter.params.size() != 1) {
                    throw std::runtime_error("Edge Detection filter needs 1 parameter: threshold\n");
                } else if (!IsFloat(filter.params[0])) {
                    throw std::runtime_error("Edge Detection filter parameter (threshold) must be a float number\n");
                } else if (std::stof(filter.params[0]) > 1 || std::stof(filter.params[0]) < 0.0) {
                    throw std::runtime_error("Threshold must be between 0.0 and 1.0\n");
                }
            } else if (filter.name == "-blur") {
                if (filter.params.size() != 1) {
                    throw std::runtime_error("Gaussian Blur filter has only 1 parameter: sigma\n");
                } else if (!IsFloat(filter.params[0])) {
                    throw std::runtime_error("Sigma parameter must be a float number\n");
                }
            } else if (filter.name == "-contr") {
                if (!filter.params.empty()) {
                    throw std::runtime_error("Auto Contrast filter doesn't have any parameters\n");
                }
            } else if (filter.name == "-gamma") {
                if (filter.params.size() != 1) {
                    throw std::runtime_error("Gamma filter has only 1 parameter: sigma\n");
                } else if (!IsFloat(filter.params[0])) {
                    throw std::runtime_error("Sigma parameter must be between 0.1 and 1.0\n");
                } else if (std::stof(filter.params[0]) > 1 || std::stof(filter.params[0]) < 0.1) {
                    throw std::runtime_error("Sigma parameter must be between 0.1 and 1.0\n");
                }
            } else if (filter.name == "-pixel") {
                if (filter.params.size() != 1) {
                    throw std::runtime_error("PixelImage filter has only 1 parameter: pixel size\n");
                } else if (!IsAllDigits(filter.params[0])) {
                    throw std::runtime_error("Pixel size must be an integer, less then the size of the image\n");
                }
            } else if (filter.name == "-crystal") {
                if (filter.params.size() != 1) {
                    throw std::runtime_error("Crystallization filter has only 1 parameter: shard size\n");
                } else if (!IsAllDigits(filter.params[0])) {
                    throw std::runtime_error("Shard size must be an integer, less then the size of the image\n");
                }
            } else {
                throw std::runtime_error(
                        "The filter you have chosen is not supported by ImageProcessor.\n"
                        "You can only choose filters from the following list:\n"
                        "1.Crop (print -crop width height)\n"
                        "2.Grayscale (print -gs)\n"
                        "3.Negative (print -neg)\n"
                        "4.Sharpening (print -sharp)\n"
                        "5.Edge Detection (print -edge threshold)\n"
                        "6.Gaussian Blur (print -blur sigma)\n"
                        "7.Auto Contrast (print -contr)\n"
                        "8.Gamma (print -gamma sigma)\n"
                        "9.PixelImage (print -pixel pixel size)\n"
                        "10.Crystallization (print -crystal shard size)\n"
                        "Remember that you can use multiple filters at once\n");
            }
        }
        return results;
    }
}