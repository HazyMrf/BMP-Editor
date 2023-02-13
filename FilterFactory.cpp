#include "FilterFactory.h"

std::shared_ptr<Filter> FilterFactory::CreateFilter(const FilterInfo &filter) {
    if (filter.name == "-crop") {
        size_t width = std::stoull(filter.params[0]);
        size_t height = std::stoull(filter.params[1]);
        return std::make_shared<Crop>(width, height);
    }
    if (filter.name == "-gs") {
        return std::make_shared<Grayscale>();
    }
    if (filter.name == "-neg") {
        return std::make_shared<Negative>();
    }
    if (filter.name == "-sharp") {
        return std::make_shared<Sharpening>();
    }
    if (filter.name == "-edge") {
        float threshold = std::stof(filter.params[0]);
        return std::make_shared<EdgeDetection>(threshold);
    }
    if (filter.name == "-blur") {
        float sigma = std::stof(filter.params[0]);
        return std::make_shared<GaussianBlur>(sigma);
    }
    if (filter.name == "-contr") {
        return std::make_shared<AutoContrast>();
    }
    if (filter.name == "-gamma") {
        float sigma = std::stof(filter.params[0]);
        return std::make_shared<Gamma>(sigma);
    }
    if (filter.name == "-pixel") {
        size_t pixel_size = std::stoull(filter.params[0]);
        return std::make_shared<PixelImage>(pixel_size);
    }
    if (filter.name == "-crystal") {
        size_t shard_size = std::stoull(filter.params[0]);
        return std::make_shared<Crystallization>(shard_size);
    }
    return {};
}

std::vector<std::shared_ptr<Filter>> FilterFactory::CreateFilters(const std::vector<FilterInfo> &filters) {
    std::vector<std::shared_ptr<Filter>> created_filters;
    created_filters.reserve(filters.size());
    for (const auto &filter: filters) {
        created_filters.push_back(CreateFilter(filter));
    }

    return created_filters;
}

void FilterFactory::ApplyFilters(Image &image, const std::vector<FilterInfo> &filters) {
    const auto& created_filters = FilterFactory::CreateFilters(filters);
    for (const auto &filter: created_filters) {
        filter->Apply(image);
    }
}
