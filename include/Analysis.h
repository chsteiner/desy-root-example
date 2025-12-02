#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <string>
#include <optional>
#include <ROOT/RDataFrame.hxx>
#include <TH1D.h>

/**
 * @brief Configuration for the dimuon analysis
 */
struct AnalysisConfig {
    std::string inputFile;
    std::string outputFile = "output.root";
    std::string outputDir = ".";
    std::optional<Long64_t> maxEvents = std::nullopt;
};

/**
 * @brief Main analysis class for dimuon event selection
 *
 * Performs event selection requiring:
 * - HLT_IsoMu24 OR HLT_IsoMu18 trigger
 * - Exactly 2 opposite-sign muons
 * - High-quality muon selection
 */
class DimuonAnalysis {
public:
    explicit DimuonAnalysis(const AnalysisConfig& config);

    /**
     * @brief Run the full analysis chain
     */
    void run();

private:
    AnalysisConfig m_config;

    /**
     * @brief Define muon quality cuts
     */
    ROOT::RDF::RNode applyMuonSelection(ROOT::RDF::RNode df);

    /**
     * @brief Apply trigger selection
     */
    ROOT::RDF::RNode applyTriggerSelection(ROOT::RDF::RNode df);

    /**
     * @brief Select events with exactly 2 opposite-sign muons
     */
    ROOT::RDF::RNode applyDimuonSelection(ROOT::RDF::RNode df);

    /**
     * @brief Calculate dimuon invariant mass
     */
    ROOT::RDF::RNode calculateInvariantMass(ROOT::RDF::RNode df);

    /**
     * @brief Book and fill histograms
     */
    void bookHistograms(ROOT::RDF::RNode df);

    /**
     * @brief Save histograms to ROOT file and PNG
     */
    void saveOutput();
};

/**
 * @brief Parse command line arguments
 */
AnalysisConfig parseArgs(int argc, char* argv[]);

#endif // ANALYSIS_H
