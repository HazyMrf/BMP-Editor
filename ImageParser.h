#include <string>
#include <vector>
#include <tuple>

struct FilterInfo {
    std::string name;
    std::vector<std::string> params;

    bool operator==(const FilterInfo& other) const;
};

struct ParserResults {
    std::string input_file_path;
    std::string output_file_path;
    std::vector<FilterInfo> filters;

    bool operator==(const ParserResults& other) const;
};

class ImageParser {
public:
    static ParserResults Parse(int argc, const char* argv[]);
};