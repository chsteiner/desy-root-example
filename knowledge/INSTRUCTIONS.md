# Implementation Plan

Execute these steps in order. **Stop and ask for confirmation if a critical error occurs.**

---

## Phase 0: Environment Verification (CRITICAL - DO FIRST)

### 0.1 Check if Running in WSL
```bash
uname -a  # Should show "Linux" and "microsoft" or "WSL"
```

If on Windows native, switch to WSL:
```bash
wsl
```

### 0.2 Check Conda Environment
```bash
which conda
conda --version
```

**If conda not found:** Follow `SETUP.md` to install Miniforge.

### 0.3 Activate/Create Environment
```bash
# Try to activate
conda activate hep-analysis

# If environment doesn't exist, create it:
conda create -n hep-analysis python=3.11 root cmake pyhf make -c conda-forge -y
conda activate hep-analysis
```

### 0.4 Verify Core Tools
```bash
# All of these must succeed:
which root
which cmake
which make
python -c "import ROOT; print('ROOT OK')"
python -c "import pyhf; print('pyhf OK')"
```

**Do NOT proceed until all checks pass.**

---

## Phase 1: Data Verification

### 1.1 Locate Data File
```bash
ls -la data.root  # Or full path if needed
```

### 1.2 Inspect Available Branches
Create `check_data.py`:
```python
import uproot

f = uproot.open("data.root")
tree = f["Events"]

print(f"Total events: {tree.num_entries}")
print(f"\nAvailable branches ({len(tree.keys())} total):")

# Check key branches
key_branches = [
    "nMuon", "Muon_pt", "Muon_eta", "Muon_phi", "Muon_mass",
    "Muon_charge", "Muon_tightId", "Muon_mediumId",
    "Muon_pfRelIso04_all", "HLT_IsoMu24", "HLT_IsoMu18", "nJet"
]

for branch in key_branches:
    status = "✓" if branch in tree.keys() else "✗ MISSING"
    print(f"  {status} {branch}")
```

Run: `python check_data.py`

**Record which branches exist** - adjust code accordingly.

---

## Phase 2: Project Skeleton

### 2.1 Create Directory Structure
```bash
mkdir -p src include build
```

### 2.2 Create CMakeLists.txt
Create a robust CMakeLists.txt that:
- Sets C++17 standard
- Finds ROOT with required components
- Links all necessary libraries
- Enables compiler warnings

### 2.3 Test CMake Configuration
```bash
cd build
cmake ..
```

**Expected:** "Configuring done" with no errors. ROOT version should be printed.

---

## Phase 3: C++ Analysis Implementation

### 3.1 Create Header File (`include/Analysis.h`)
Define:
- Configuration struct (input file, output file, max events)
- Analysis class with public `run()` method

### 3.2 Create Implementation (`src/Analysis.cpp`)
Implement the analysis chain using RDataFrame:

1. **Trigger Selection**
   - Filter on `HLT_IsoMu24 || HLT_IsoMu18`

2. **Muon Quality Selection**
   - Define mask for good muons (pT, eta, ID, isolation)
   - Count good muons

3. **Dimuon Selection**
   - Require exactly 2 good muons
   - Require opposite charges

4. **Invariant Mass Calculation**
   - Use `ROOT::Math::PtEtaPhiMVector` for 4-vectors
   - Sum vectors and extract mass

5. **Histogram Booking**
   - Book histograms for nJet, muon pT, dimuon mass
   - Use named filters for cutflow

6. **Output**
   - Save histograms to ROOT file
   - Save plots as PNG

### 3.3 Create Main Entry Point (`src/main.cpp`)
- Parse command line arguments (-n, -i, -o, -h)
- Create Analysis object and call run()
- Handle exceptions gracefully

### 3.4 Compile
```bash
cd build
cmake ..
make -j4
```

**Must compile without errors.**

### 3.5 Validation Run
```bash
./dimuon_analysis -i ../data.root -n 10000
```

**Check:**
- [ ] Program completes without crash
- [ ] Cutflow is printed
- [ ] PNG files are created
- [ ] output.root is created

---

## Phase 4: Python Limit Setting

### 4.1 Create `limit_setting.py`
Implement:
- Model creation with pyhf
- Single-bin counting experiment
- CLs calculation at μ=1
- Upper limit scan

### 4.2 Test
```bash
python limit_setting.py
```

**Expected output:**
- CLs value printed
- 95% CL upper limit printed
- No errors

---

## Phase 5: Documentation

### 5.1 Update/Create README.md
Include:
- Project description
- Prerequisites
- Build instructions
- Run instructions with examples
- Output description

### 5.2 Final Validation
Run complete workflow:
```bash
# Build
cd build && cmake .. && make -j4

# Run analysis (quick test)
./dimuon_analysis -i ../data.root -n 100000

# Run limit setting
cd .. && python limit_setting.py

# Check outputs
ls -la build/*.png build/*.root
```

---

## Troubleshooting Quick Reference

| Error | Solution |
|:------|:---------|
| `cmake: command not found` | `conda install cmake -c conda-forge` |
| `make: command not found` | `conda install make -c conda-forge` |
| `ROOT not found` | Check `conda activate hep-analysis` |
| `Range + ImplicitMT error` | Disable MT when using `-n` flag |
| `Branch not found` | Run `check_data.py` to verify names |
| `Permission denied` | Check file exists and path is correct |

---

## Success Criteria

The implementation is complete when:

1. ✅ `cmake .. && make` compiles cleanly
2. ✅ `./dimuon_analysis -n 10000` runs without errors
3. ✅ Cutflow shows reasonable event counts
4. ✅ `dimuon_mass.png` shows Z peak at ~91 GeV
5. ✅ `python limit_setting.py` prints CLs values
6. ✅ README contains all build/run instructions
