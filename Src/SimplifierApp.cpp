#include "SimplifierApp.hpp"
#include "Extras.hpp"
#include "STL.hpp"
#include "Simplify.hpp"
#include <stdexcept>

namespace fs = std::filesystem;

void SimplifierApp::PrintUsage () const {
    l.Log (R"""(Usage:
    example: 
        Simplifier.exe factor=0.1 in=input.stl out=output.stl
        Simplifier.exe in=d:\Downloads\39-stl\stl\Dragon.stl mode=iterative iterations=5
    params:
        - in: input file path
        - out: output file path             [optional, default=input_simplified<iteration>.stl]
        - factor: 0.01-0.99                 [optional, default=0.5]
        - mode: simple|iterative            [optional, default=simple]
        - iterations: number of iterations  [optional, default=1] (only for iterative mode)
)""");
}

int SimplifierApp::ParseParams (size_t argc, char *argv[]) {
    for (size_t i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.find ("in=") == 0) {
            params.inputPath = arg.substr (3);
        } else if (arg.find ("out=") == 0) {
            params.outputPath = arg.substr (4);
        } else if (arg.find ("factor=") == 0) {
            params.factor = std::stod (arg.substr (7));
        } else if (arg.find ("mode=") == 0) {
            std::string mode = arg.substr (5);
            if (mode == "simple") {
                params.mode = Params::Mode::Simple;
            } else if (mode == "iterative") {
                params.mode = Params::Mode::Iterative;
            } else {
                l.Error ("Unknown mode: ", mode);
                return 1;
            }
        } else if (arg.find ("iterations=") == 0) {
            params.iterations = std::stoi (arg.substr (11));
        } else {
            l.Error ("Unknown argument: ", arg);
            return 1;
        }
    }

    if (params.inputPath.empty ()) {
        l.Error ("Input file path is required");
        return 1;
    }
    if (params.iterations < 1) {
        l.Error ("Invalid number of iterations: ", params.iterations);
        return 1;
    }
    if (params.factor <= 0 || params.factor >= 1) {
        l.Error ("Invalid factor: ", params.factor);
        return 1;
    }
    if (!fs::exists (params.inputPath) || !fs::directory_entry{params.inputPath}.is_regular_file ()) {
        l.Error ("Invalid inputPath: ", params.inputPath.string ());
        return 1;
    }
    if (!params.outputPath.empty () && (!fs::exists (params.outputPath) || !fs::directory_entry{params.outputPath}.is_regular_file ())) {
        l.Error ("Invalid outputPath: ", params.outputPath.string ());
        return 1;
    }

    return 0;
}

void SimplifierApp::RunSimpleMode () {
    try {
        l.Log ("Loading ", params.inputPath);
        Mesh mesh = STL::LoadBinary (params.inputPath);

        l.Log ("Input mesh contains ", mesh.size (), " faces");
        l.Log ("Simplifying to ", static_cast<int> (params.factor * 100), "% of original...");
        Mesh simplifiedMesh;
        long long dur = TimeIt ([&mesh, &simplifiedMesh, factor = params.factor] () {
            simplifiedMesh = Simplify (mesh, factor);
        });

        l.Log ("Simplification took  ", dur, " ms");
        l.Log ("Output mesh contains ", simplifiedMesh.size (), " faces. Actual factor: ", static_cast<double> (simplifiedMesh.size ()) / mesh.size ());

        if (params.outputPath.empty ()) {
            params.outputPath = params.inputPath;
            params.outputPath.replace_filename (params.inputPath.stem ().string () + "_simplified.stl");
        }

        l.Log ("Writing ", params.outputPath.string ());
        STL::SaveBinary (params.outputPath, simplifiedMesh);

    } catch (const std::exception &e) {
        l.Error (e.what ());
        return;
    }
}

void SimplifierApp::RunIterativeMode () {
    try {
        l.Log ("Loading ", params.inputPath);
        Mesh mesh = STL::LoadBinary (params.inputPath);

        l.Log ("Input mesh contains ", mesh.size (), " faces");
        l.Log ("Simplifying... ");
        std::vector<std::pair<size_t, long long>> iterationStats;

        Mesh simplifiedMesh;
        Mesh previousMesh = mesh;
        for (size_t iteration = 0; iteration < params.iterations; iteration++) {
            long long dur = TimeIt ([&previousMesh, &simplifiedMesh, factor = params.factor] () {
                simplifiedMesh = Simplify (previousMesh, factor);
            });
            iterationStats.push_back ({simplifiedMesh.size (), dur});

            if (iterationStats.size () > 2 && iterationStats[iteration].first == iterationStats[iteration - 1].first) {
                break;
            }
            auto outName = fs::path (params.inputPath).replace_filename (params.inputPath.stem ().string () + "_simplified" + std::to_string (iteration + 1) + ".stl");
            STL::SaveBinary (outName, simplifiedMesh);
            previousMesh = simplifiedMesh;
        }
        for (size_t i = 0; i < iterationStats.size (); i++)
            l.Log ("Iteration ", i + 1, " | ", iterationStats[i].first, " faces | duration: ", iterationStats[i].second, " ms");
        if (iterationStats.size () > 2 && iterationStats[iterationStats.size () - 1].first == iterationStats[iterationStats.size () - 2].first) {
            l.Log ("No further simplification possible");
        }

    } catch (const std::exception &e) {
        l.Error (e.what ());
    }
}

void SimplifierApp::Run () {
    switch (params.mode) {
    case Params::Mode::Simple:
        RunSimpleMode ();
        break;
    case Params::Mode::Iterative:
        RunIterativeMode ();
        break;
    default:
        l.Error ("Unknown mode: ", static_cast<int> (params.mode));
        break;
    }
}

bool SimplifierApp::Init (int argc, char *argv[]) {
    if (ParseParams (argc, argv)) {
        PrintUsage ();
        return false;
    }
    return true;
}
