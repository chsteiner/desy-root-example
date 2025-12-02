#include "Analysis.h"

#include <iostream>
#include <cstdlib>
#include <getopt.h>

#include <TFile.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLatex.h>
#include <Math/Vector4D.h>

// Use ROOT's VecOps for vectorized operations
using namespace ROOT::VecOps;

DimuonAnalysis::DimuonAnalysis(const AnalysisConfig& config)
    : m_config(config) {}

void DimuonAnalysis::run() {
    std::cout << "=== Dimuon Analysis ===" << std::endl;
    std::cout << "Input file: " << m_config.inputFile << std::endl;

    // Enable multithreading only if not using Range (they are incompatible)
    if (!m_config.maxEvents.has_value()) {
        ROOT::EnableImplicitMT();
        std::cout << "Multithreading enabled" << std::endl;
    } else {
        std::cout << "Processing first " << m_config.maxEvents.value() << " events (single-threaded mode)" << std::endl;
    }

    // Create RDataFrame
    ROOT::RDataFrame df("Events", m_config.inputFile);

    // Apply event limit if specified
    ROOT::RDF::RNode node = df;
    if (m_config.maxEvents.has_value()) {
        node = node.Range(0, m_config.maxEvents.value());
    }

    // Apply analysis chain
    auto triggered = applyTriggerSelection(node);
    auto selected = applyMuonSelection(triggered);
    auto dimuon = applyDimuonSelection(selected);
    auto withMass = calculateInvariantMass(dimuon);

    // Book histograms and run
    bookHistograms(withMass);
}

ROOT::RDF::RNode DimuonAnalysis::applyTriggerSelection(ROOT::RDF::RNode df) {
    // Check if trigger branches exist, then apply OR of HLT_IsoMu24 and HLT_IsoMu18
    // NanoAOD stores triggers as Bool_t branches
    auto triggered = df.Define("passTrigger",
        [](Bool_t isoMu24, Bool_t isoMu18) {
            return isoMu24 || isoMu18;
        },
        {"HLT_IsoMu24", "HLT_IsoMu18"}
    ).Filter("passTrigger", "Trigger selection (HLT_IsoMu24 || HLT_IsoMu18)");

    return triggered;
}

ROOT::RDF::RNode DimuonAnalysis::applyMuonSelection(ROOT::RDF::RNode df) {
    // Define good muon mask based on quality criteria
    // Standard CMS muon ID cuts for NanoAOD
    auto withGoodMuons = df.Define("goodMuon_mask",
        [](const RVec<Float_t>& pt,
           const RVec<Float_t>& eta,
           const RVec<Bool_t>& tightId,
           const RVec<Float_t>& pfRelIso04_all) {
            // Muon selection:
            // - pT > 20 GeV
            // - |eta| < 2.4
            // - Tight ID
            // - PF relative isolation < 0.15
            return (pt > 20.0f) &&
                   (abs(eta) < 2.4f) &&
                   tightId &&
                   (pfRelIso04_all < 0.15f);
        },
        {"Muon_pt", "Muon_eta", "Muon_tightId", "Muon_pfRelIso04_all"}
    );

    // Count good muons
    auto withCount = withGoodMuons.Define("nGoodMuon",
        [](const RVec<int>& mask) {
            return static_cast<int>(Sum(mask));
        },
        {"goodMuon_mask"}
    );

    // Define filtered muon collections
    auto withFiltered = withCount
        .Define("goodMuon_pt", "Muon_pt[goodMuon_mask]")
        .Define("goodMuon_eta", "Muon_eta[goodMuon_mask]")
        .Define("goodMuon_phi", "Muon_phi[goodMuon_mask]")
        .Define("goodMuon_mass", "Muon_mass[goodMuon_mask]")
        .Define("goodMuon_charge", "Muon_charge[goodMuon_mask]");

    return withFiltered;
}

ROOT::RDF::RNode DimuonAnalysis::applyDimuonSelection(ROOT::RDF::RNode df) {
    // Require exactly 2 good muons
    auto twoMuons = df.Filter("nGoodMuon == 2", "Exactly 2 good muons");

    // Require opposite sign
    auto oppSign = twoMuons.Filter(
        [](const RVec<Int_t>& charge) {
            return charge[0] * charge[1] < 0;
        },
        {"goodMuon_charge"},
        "Opposite-sign muons"
    );

    return oppSign;
}

ROOT::RDF::RNode DimuonAnalysis::calculateInvariantMass(ROOT::RDF::RNode df) {
    // Calculate dimuon invariant mass using 4-vectors
    auto withMass = df.Define("dimuon_mass",
        [](const RVec<Float_t>& pt,
           const RVec<Float_t>& eta,
           const RVec<Float_t>& phi,
           const RVec<Float_t>& mass) {
            // Create 4-vectors for both muons
            ROOT::Math::PtEtaPhiMVector mu1(pt[0], eta[0], phi[0], mass[0]);
            ROOT::Math::PtEtaPhiMVector mu2(pt[1], eta[1], phi[1], mass[1]);

            // Return invariant mass of the dimuon system
            return static_cast<float>((mu1 + mu2).M());
        },
        {"goodMuon_pt", "goodMuon_eta", "goodMuon_phi", "goodMuon_mass"}
    );

    // Also define individual muon momenta for plotting
    auto withMomenta = withMass
        .Define("muon1_pt", "goodMuon_pt[0]")
        .Define("muon2_pt", "goodMuon_pt[1]");

    return withMomenta;
}

void DimuonAnalysis::bookHistograms(ROOT::RDF::RNode df) {
    std::cout << "Booking histograms..." << std::endl;

    // Set ROOT style
    gStyle->SetOptStat(1111);
    gStyle->SetOptFit(0);

    // Book histograms
    auto h_nJet = df.Histo1D(
        {"h_nJet", "Number of Jets;Number of jets;Events", 15, 0, 15},
        "nJet"
    );

    auto h_muon1_pt = df.Histo1D(
        {"h_muon1_pt", "Leading Muon p_{T};p_{T} [GeV];Events / 5 GeV", 40, 0, 200},
        "muon1_pt"
    );

    auto h_muon2_pt = df.Histo1D(
        {"h_muon2_pt", "Subleading Muon p_{T};p_{T} [GeV];Events / 5 GeV", 40, 0, 200},
        "muon2_pt"
    );

    auto h_dimuon_mass = df.Histo1D(
        {"h_dimuon_mass", "Dimuon Invariant Mass;m_{#mu#mu} [GeV];Events / 2 GeV", 75, 0, 150},
        "dimuon_mass"
    );

    // Trigger the event loop
    std::cout << "Running event loop..." << std::endl;

    // Get cutflow report
    auto report = df.Report();

    // Force histogram computation
    auto nEvents = h_dimuon_mass->GetEntries();
    std::cout << "Selected " << nEvents << " dimuon events" << std::endl;

    // Print cutflow
    std::cout << "\n=== Cutflow ===" << std::endl;
    report->Print();

    // Save to ROOT file
    std::cout << "\nSaving histograms to " << m_config.outputFile << std::endl;
    TFile outFile(m_config.outputFile.c_str(), "RECREATE");
    h_nJet->Write();
    h_muon1_pt->Write();
    h_muon2_pt->Write();
    h_dimuon_mass->Write();
    outFile.Close();

    // Save PNG plots
    auto savePlot = [this](TH1D* hist, const std::string& name) {
        TCanvas canvas("c", "c", 800, 600);
        canvas.SetLeftMargin(0.12);
        canvas.SetRightMargin(0.05);

        hist->SetLineColor(kBlue);
        hist->SetLineWidth(2);
        hist->SetFillColor(kBlue - 9);
        hist->Draw("HIST");

        // Add CMS label
        TLatex latex;
        latex.SetNDC();
        latex.SetTextFont(62);
        latex.SetTextSize(0.05);
        latex.DrawLatex(0.12, 0.92, "CMS Open Data");

        std::string outPath = m_config.outputDir + "/" + name + ".png";
        canvas.SaveAs(outPath.c_str());
        std::cout << "Saved " << outPath << std::endl;
    };

    savePlot(h_nJet.GetPtr(), "nJet");
    savePlot(h_muon1_pt.GetPtr(), "muon1_pt");
    savePlot(h_muon2_pt.GetPtr(), "muon2_pt");
    savePlot(h_dimuon_mass.GetPtr(), "dimuon_mass");

    std::cout << "\nAnalysis complete!" << std::endl;
}

AnalysisConfig parseArgs(int argc, char* argv[]) {
    AnalysisConfig config;
    config.inputFile = "data.root";  // Default input file

    static struct option long_options[] = {
        {"input", required_argument, 0, 'i'},
        {"output", required_argument, 0, 'o'},
        {"outdir", required_argument, 0, 'd'},
        {"nevents", required_argument, 0, 'n'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "i:o:d:n:h", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'i':
                config.inputFile = optarg;
                break;
            case 'o':
                config.outputFile = optarg;
                break;
            case 'd':
                config.outputDir = optarg;
                break;
            case 'n':
                config.maxEvents = std::atoll(optarg);
                break;
            case 'h':
            default:
                std::cout << "Usage: " << argv[0] << " [options]\n"
                          << "Options:\n"
                          << "  -i, --input FILE    Input ROOT file (default: data.root)\n"
                          << "  -o, --output FILE   Output ROOT file (default: output.root)\n"
                          << "  -d, --outdir DIR    Output directory for plots (default: .)\n"
                          << "  -n, --nevents N     Process only first N events\n"
                          << "  -h, --help          Show this help message\n";
                std::exit(opt == 'h' ? 0 : 1);
        }
    }

    return config;
}
