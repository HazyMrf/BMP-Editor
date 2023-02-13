#include "catch.hpp"
#include "ImageParser.h"

TEST_CASE("Parsing Basic Cases") {

    SECTION("No-Filter Basic Case") {

        const char* argv[] = {"./image_processor", "input_file", "output_file"};

        REQUIRE(ImageParser::Parse(3, argv) == ParserResults{"input_file", "output_file", {}});
    }

    SECTION("Handling Basic Exception Cases") {
        const char* argv1[] = {"./image_processor"};

        REQUIRE_THROWS(ImageParser::Parse(1, argv1));

        const char* argv2[] = {"./image_processor", "random_file"};

        REQUIRE_THROWS_WITH(ImageParser::Parse(2, argv2), "You need to write input and output files\n");

        const char* argv4[] = {"./image_processor", "example_file", "example_file", "filter1", "params"};

        REQUIRE_THROWS_WITH(ImageParser::Parse(4, argv4),
                            "input and output file should be different files, so that input is unchanged\n");
    }

    SECTION("Params without Filter Name Case") {
        const char* argv[] = {"./image_processor", "input_file", "output_file", "random_no_hyphen_word"};

        REQUIRE_THROWS_WITH(ImageParser::Parse(4, argv), "You haven't passed the name of the filter\n");
    }
}

TEST_CASE("Parsing Possible Image-Changing Cases") {

    SECTION("Parsing Each Filter") {

        SECTION("Crop") {

            const char* argv_not_2[] = {"./image_processor", "input", "output", "-crop", "param1", "param2", "param3"};

            REQUIRE_THROWS_WITH(ImageParser::Parse(7, argv_not_2), "Crop filter has 2 parameters: width and height\n");

            const char* argv_not_digit[] = {"./image_processor", "input", "output", "-crop", "800", "not digit"};

            REQUIRE_THROWS_WITH(ImageParser::Parse(6, argv_not_digit),
                                "Crop filter parameters (width and height) must be integers\n");

            const char* argv[] = {"./image_processor,", "input", "output", "-crop", "600", "800"};

            REQUIRE_NOTHROW(ImageParser::Parse(6, argv));
            REQUIRE(ImageParser::Parse(6, argv) == ParserResults{"input", "output", {{"-crop", {"600", "800"}}}});
        }

        SECTION("Grayscale") {

            const char* argv_not_empty[] = {"./image_processor", "input", "output", "-gs", "param1", "param2"};

            REQUIRE_THROWS_WITH(ImageParser::Parse(6, argv_not_empty),
                                "Grayscale filter doesn't have any parameters\n");

            const char* argv[] = {"./image_processor,", "input", "output", "-gs"};

            REQUIRE_NOTHROW(ImageParser::Parse(4, argv));
            REQUIRE(ImageParser::Parse(4, argv) == ParserResults{"input", "output", {{"-gs", {}}}});
        }

        SECTION("Negative") {

            const char* argv_not_empty[] = {"./image_processor", "input", "output", "-neg", "param1", "param2"};

            REQUIRE_THROWS_WITH(ImageParser::Parse(6, argv_not_empty), "Negative filter doesn't have any parameters\n");

            const char* argv[] = {"./image_processor,", "input", "output", "-neg"};

            REQUIRE_NOTHROW(ImageParser::Parse(4, argv));
            REQUIRE(ImageParser::Parse(4, argv) == ParserResults{"input", "output", {{"-neg", {}}}});
        }

        SECTION("Sharpening") {

            const char* argv_not_empty[] = {"./image_processor", "input", "output", "-sharp", "param"};

            REQUIRE_THROWS_WITH(ImageParser::Parse(5, argv_not_empty),
                                "Sharpening filter doesn't have any parameters\n");

            const char* argv[] = {"./image_processor,", "input", "output", "-sharp"};

            REQUIRE_NOTHROW(ImageParser::Parse(4, argv));
            REQUIRE(ImageParser::Parse(4, argv) == ParserResults{"input", "output", {{"-sharp", {}}}});
        }

        SECTION("Edge Detection") {

            const char* argv_not_1[] = {"./image_processor", "input", "output", "-edge", "param1", "param2", "param3"};

            REQUIRE_THROWS_WITH(ImageParser::Parse(7, argv_not_1),
                                "Edge Detection filter needs 1 parameter: threshold\n");

            const char* argv_not_float[] = {"./image_processor", "input", "output", "-edge", "123not_float*"};

            REQUIRE_THROWS_WITH(ImageParser::Parse(5, argv_not_float),
                                "Edge Detection filter parameter (threshold) must be a float number\n");

            const char* argv_too_big[] = {"./image_processor", "input", "output", "-edge", "1.1"};

            REQUIRE_THROWS_WITH(ImageParser::Parse(5, argv_too_big), "Threshold must be between 0.0 and 1.0\n");

            const char* argv[] = {"./image_processor,", "input", "output", "-edge", "0.77"};

            REQUIRE_NOTHROW(ImageParser::Parse(5, argv));
            REQUIRE(ImageParser::Parse(5, argv) == ParserResults{"input", "output", {{"-edge", {"0.77"}}}});
        }

        SECTION("Gaussian Blur") {

            const char* argv_not_1[] = {"./image_processor", "input", "output", "-blur", "param1", "param2"};

            REQUIRE_THROWS_WITH(ImageParser::Parse(6, argv_not_1),
                                "Gaussian Blur filter has only 1 parameter: sigma\n");

            const char* argv_not_float[] = {"./image_processor", "input", "output", "-blur", "777.float?"};

            REQUIRE_THROWS_WITH(ImageParser::Parse(5, argv_not_float), "Sigma parameter must be a float number\n");

            const char* argv[] = {"./image_processor,", "input", "output", "-blur", "4.5"};

            REQUIRE_NOTHROW(ImageParser::Parse(5, argv));
            REQUIRE(ImageParser::Parse(5, argv) == ParserResults{"input", "output", {{"-blur", {"4.5"}}}});
        }

        SECTION("AutoContrast") {

            const char* argv_not_empty[] = {"./image_processor", "input", "output", "-contr", "param", "-filter"};

            REQUIRE_THROWS_WITH(ImageParser::Parse(6, argv_not_empty),
                                "Auto Contrast filter doesn't have any parameters\n");

            const char* argv[] = {"./image_processor,", "input", "output", "-contr"};

            REQUIRE_NOTHROW(ImageParser::Parse(4, argv));
            REQUIRE(ImageParser::Parse(4, argv) == ParserResults{"input", "output", {{"-contr", {}}}});
        }

        SECTION("Gamma") {

            const char* argv_not_1[] = {"./image_processor", "input", "output", "-gamma", "param1", "param2"};

            REQUIRE_THROWS_WITH(ImageParser::Parse(6, argv_not_1), "Gamma filter has only 1 parameter: sigma\n");

            const char* argv_not_float[] = {"./image_processor", "input", "output", "-gamma", "8.8.0"};

            REQUIRE_THROWS_WITH(ImageParser::Parse(5, argv_not_float), "Sigma parameter must be between 0.1 and 1.0\n");

            const char* argv_too_big[] = {"./image_processor", "input", "output", "-gamma", "1.1"};

            REQUIRE_THROWS_WITH(ImageParser::Parse(5, argv_too_big), "Sigma parameter must be between 0.1 and 1.0\n");

            const char* argv[] = {"./image_processor,", "input", "output", "-gamma", "0.777"};

            REQUIRE_NOTHROW(ImageParser::Parse(5, argv));
            REQUIRE(ImageParser::Parse(5, argv) == ParserResults{"input", "output", {{"-gamma", {"0.777"}}}});
        }

        SECTION("PixelImage") {

            const char* argv_not_1[] = {"./image_processor", "input", "output", "-pixel", "C++", "Python"};

            REQUIRE_THROWS_WITH(ImageParser::Parse(6, argv_not_1),
                                "PixelImage filter has only 1 parameter: pixel size\n");

            const char* argv_not_int[] = {"./image_processor", "input", "output", "-pixel", "123.45"};

            REQUIRE_THROWS_WITH(ImageParser::Parse(5, argv_not_int),
                                "Pixel size must be an integer, less then the size of the image\n");

            const char* argv[] = {"./image_processor,", "input", "output", "-pixel", "37"};

            REQUIRE_NOTHROW(ImageParser::Parse(5, argv));
            REQUIRE(ImageParser::Parse(5, argv) == ParserResults{"input", "output", {{"-pixel", {"37"}}}});
        }

        SECTION("Crystallize") {

            const char* argv_not_1[] = {"./image_processor", "input", "output", "-crystal", "Java", "Ruby"};

            REQUIRE_THROWS_WITH(ImageParser::Parse(6, argv_not_1),
                                "Crystallization filter has only 1 parameter: shard size\n");

            const char* argv_not_int[] = {"./image_processor", "input", "output", "-crystal", "123.45"};

            REQUIRE_THROWS_WITH(ImageParser::Parse(5, argv_not_int),
                                "Shard size must be an integer, less then the size of the image\n");

            const char* argv[] = {"./image_processor,", "input", "output", "-crystal", "37"};

            REQUIRE_NOTHROW(ImageParser::Parse(5, argv));
            REQUIRE(ImageParser::Parse(5, argv) == ParserResults{"input", "output", {{"-crystal", {"37"}}}});
        }

        SECTION("Invalid Filters") {
            const char* argv_invalid1[] = {"./image_processor", "input", "output", "-filter", "param1", "param2"};

            REQUIRE_THROWS(ImageParser::Parse(6, argv_invalid1));

            const char* argv_invalid2[] = {"./image_processor", "input", "output", "-gs", "-sharp", "-emboss"};

            REQUIRE_THROWS(ImageParser::Parse(6, argv_invalid2));

            const char* argv_invalid3[] = {
                "./image_processor", "input", "output", "-crop", "800", "600", "-gs", "-gamma", "0.7", "-trash"};

            REQUIRE_THROWS(ImageParser::Parse(10, argv_invalid3));

            const char* argv_invalid4[] = {"./image_processor", "input", "output", "-grayscale", "-sharpening"};

            REQUIRE_THROWS(ImageParser::Parse(5, argv_invalid4));

            const char* argv_invalid5[] = {"./image_processor", "input", "output",  "-sharp",
                                           "-crystal",          "32",    "-Gandalf"};

            REQUIRE_THROWS(ImageParser::Parse(7, argv_invalid5));
        }
    }

    SECTION("Parsing Valid Inputs") {
        const char* argv_valid1[] = {"./image_processor", "input", "output", "-sharp", "-gs"};

        REQUIRE(ImageParser::Parse(5, argv_valid1) == ParserResults{"input", "output", {{"-sharp", {}}, {"-gs", {}}}});

        const char* argv_valid2[] = {"./image_processor", "input", "output", "-sharp", "-gs", "-gamma", "0.777"};

        REQUIRE(ImageParser::Parse(7, argv_valid2) ==
                ParserResults{"input", "output", {{"-sharp", {}}, {"-gs", {}}, {"-gamma", {"0.777"}}}});

        const char* argv_valid3[] = {"./image_processor", "input", "output", "-crop", "800", "600", "-gs",
                                     "-crystal",          "32"};
        REQUIRE(ImageParser::Parse(9, argv_valid3) ==
                ParserResults{"input", "output", {{"-crop", {"800", "600"}}, {"-gs", {}}, {"-crystal", {"32"}}}});

        const char* argv_valid4[] = {"./image_processor", "input", "output", "-neg", "-edge", "0.567"};

        REQUIRE(ImageParser::Parse(6, argv_valid4) ==
                ParserResults{"input", "output", {{"-neg", {}}, {"-edge", {"0.567"}}}});

        const char* argv_valid5[] = {
            "./image_processor", "input", "output", "-blur", "10.1", "-gs", "-blur", "23", "-contr"};

        REQUIRE(
            ImageParser::Parse(9, argv_valid5) ==
            ParserResults{"input", "output", {{"-blur", {"10.1"}}, {"-gs", {}}, {"-blur", {"23"}}, {"-contr", {}}}});

        const char* argv_valid6[] = {
            "./image_processor", "input", "output", "-contr", "-crop", "777", "7777", "-neg", "-blur", "40",
            "-crystal",          "32"};

        REQUIRE(
            ImageParser::Parse(12, argv_valid6) ==
            ParserResults{
                "input",
                "output",
                {{"-contr", {}}, {"-crop", {"777", "7777"}}, {"-neg", {}}, {"-blur", {"40"}}, {"-crystal", {"32"}}}});

        const char* argv_valid7[] = {"./image_processor",
                                     "input",
                                     "output",
                                     "-crop",
                                     "1000",
                                     "1000",
                                     "-blur",
                                     "777",
                                     "-edge",
                                     "0.1",
                                     "-contr",
                                     "-gamma",
                                     "0.555",
                                     "-crystal",
                                     "13"};

        REQUIRE(ImageParser::Parse(15, argv_valid7) == ParserResults{"input",
                                                                     "output",
                                                                     {{"-crop", {"1000", "1000"}},
                                                                      {"-blur", {"777"}},
                                                                      {"-edge", {"0.1"}},
                                                                      {"-contr", {}},
                                                                      {"-gamma", {"0.555"}},
                                                                      {"-crystal", {"13"}}}});
    }
}

