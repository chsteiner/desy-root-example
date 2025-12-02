#include "Analysis.h"

#include <iostream>
#include <exception>

int main(int argc, char* argv[]) {
    try {
        // Parse command line arguments
        AnalysisConfig config = parseArgs(argc, argv);

        // Create and run the analysis
        DimuonAnalysis analysis(config);
        analysis.run();

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
