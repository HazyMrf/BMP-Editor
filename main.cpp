#include "FilterFactory.h"
#include <iostream>

int main(int argc, const char* argv[]) {
    try {
        auto parser_results = ImageParser::Parse(argc, argv);
        Image image(parser_results.input_file_path);
        FilterFactory::ApplyFilters(image, parser_results.filters);
        image.Write(parser_results.output_file_path);
    } catch (std::exception& e) {
        std::cerr << e.what();
    }
    return 0;
}
