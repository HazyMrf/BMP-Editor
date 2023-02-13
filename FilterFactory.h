#include "Filter.h"
#include <memory>

class FilterFactory {
public:
    static std::shared_ptr<Filter> CreateFilter(const FilterInfo& filter);

    static std::vector<std::shared_ptr<Filter>> CreateFilters(const std::vector<FilterInfo> &filters);

    static void ApplyFilters(Image& image, const std::vector<FilterInfo>& filters);
};
