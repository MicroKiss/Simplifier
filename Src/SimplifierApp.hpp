#pragma once
#include "Logger.hpp"
#include "SimplifierApp.hpp"
#include <filesystem>

class SimplifierApp {
    struct Params {
        enum class Mode { Simple,
                          Iterative };
        double factor = 0.5;
        std::filesystem::path inputPath;
        std::filesystem::path outputPath;
        Mode mode = Mode::Simple;
        size_t iterations = 1;
    };
    Params params;
    void PrintUsage () const;
    int ParseParams (size_t argc, char *argv[]);
    void RunSimpleMode ();
    void RunIterativeMode ();
    Logger l;

  public:
    SimplifierApp () = default;
    ~SimplifierApp () = default;
    bool Init (int argc, char *argv[]);
    void Run ();
};