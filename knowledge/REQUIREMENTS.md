# Project Requirements

## 1. C++ Analysis (Core)

### Framework
* **MUST use:** ROOT with **RDataFrame** (modern C++ functional style)
* **Avoid:** Legacy TTree event loops with `GetEntry()`

### Event Selection (Filtering)

Apply these cuts in order:

1. **Trigger:** Event must fire the Single Muon Trigger
   - Use: `HLT_IsoMu24 || HLT_IsoMu18`

2. **Muon Quality:** Both muons must pass quality criteria
   - pT > 20 GeV
   - |η| < 2.4
   - Pass `Muon_tightId` (or `Muon_mediumId` if tightId unavailable)
   - PF relative isolation < 0.15 (`Muon_pfRelIso04_all < 0.15`)

3. **Dimuon Selection:**
   - Exactly **two good muons** passing quality cuts
   - Opposite electrical charges (`charge[0] * charge[1] < 0`)

### Deliverables (Plots)

| Plot | X-axis | Range | Description |
|:-----|:-------|:------|:------------|
| 1 | `nJet` | 0-15 | Number of jets per event |
| 2 | `muon_pt` | 0-200 GeV | pT of leading and sub-leading muon |
| 3 | `dimuon_mass` | 0-150 GeV | Invariant mass of μ⁺μ⁻ system |

### Output Files
* Save plots as `.png` files
* Save histograms to `output.root` (for later analysis)

### CLI Arguments
* Implement `-n <int>` flag to process only first N events (for debugging)
* Implement `-i <file>` flag to specify input file (default: `data.root`)
* Implement `-o <file>` flag to specify output ROOT file (default: `output.root`)

---

## 2. Project Structure & Build System

### Directory Structure
```
project/
├── CMakeLists.txt
├── include/
│   └── Analysis.h
├── src/
│   ├── Analysis.cpp
│   └── main.cpp
├── build/           # Created by cmake
├── data.root        # Input data
└── knowledge/       # These documentation files
```

### CMakeLists.txt Requirements
* Use `find_package(ROOT REQUIRED COMPONENTS ...)`
* Required ROOT components:
  - `Core`, `RIO`, `Tree`, `Hist`, `Gpad`, `Graf`
  - `ROOTDataFrame`, `ROOTVecOps`, `Physics`
* Set C++ standard to 17 or higher
* Enable compiler warnings (`-Wall -Wextra`)

---

## 3. Limit Setting (Python)

### Script
* Filename: `limit_setting.py`
* Library: `pyhf` (Python HistFactory)

### Dummy Counting Experiment
Create a simplified model to demonstrate limit setting:

| Parameter | Value |
|:----------|:------|
| Observed events | 100 |
| Expected background | 100 |
| Background uncertainty | ±10 (10% systematic) |
| Expected signal | 10 |

### Required Output
* Print CLs value for signal strength μ=1
* Print 95% CL upper limit on signal strength
* Print expected CLs band (±1σ, ±2σ)

---

## 4. Documentation

### README.md
Must contain:
* Project description
* Build instructions (cmake commands)
* Run instructions with examples
* Output file descriptions

---

## Acceptance Criteria

The project is complete when:
- [ ] `cmake .. && make` compiles without errors
- [ ] `./dimuon_analysis -n 10000` runs and produces PNG files
- [ ] `dimuon_mass.png` shows a clear peak around 91 GeV
- [ ] `python limit_setting.py` prints CLs values
- [ ] README contains complete build/run instructions
