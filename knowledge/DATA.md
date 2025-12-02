# Data Description

## Input File
* **Filename:** `data.root`
* **Format:** CMS NanoAOD (NANOAOD)
* **Location:** Project root directory
* **Size:** ~2.3 GB (real CMS collision data from 2016)

## Data Schema (ROOT TTree / RDataFrame)

The analysis uses `RDataFrame`. Column names follow the standard CMS NanoAOD naming convention.

### Muon Variables

| Variable | Branch Name | Type | Description |
|:---------|:------------|:-----|:------------|
| Transverse Momentum | `Muon_pt` | `Float_t[]` | pT in GeV |
| Pseudorapidity | `Muon_eta` | `Float_t[]` | η coordinate |
| Azimuthal Angle | `Muon_phi` | `Float_t[]` | φ coordinate |
| Mass | `Muon_mass` | `Float_t[]` | Muon mass (~0.105 GeV) |
| Charge | `Muon_charge` | `Int_t[]` | +1 or -1 |
| Tight ID | `Muon_tightId` | `Bool_t[]` | High-quality muon flag |
| Medium ID | `Muon_mediumId` | `Bool_t[]` | Medium-quality muon flag |
| PF Isolation | `Muon_pfRelIso04_all` | `Float_t[]` | Relative isolation (ΔR<0.4) |
| Count | `nMuon` | `UInt_t` | Number of muons in event |

### Trigger Variables

| Variable | Branch Name | Type | Description |
|:---------|:------------|:-----|:------------|
| Single Muon (24 GeV) | `HLT_IsoMu24` | `Bool_t` | Isolated muon trigger |
| Single Muon (18 GeV) | `HLT_IsoMu18` | `Bool_t` | Lower threshold trigger |

### Jet Variables

| Variable | Branch Name | Type | Description |
|:---------|:------------|:-----|:------------|
| Jet Count | `nJet` | `UInt_t` | Number of jets in event |

## Important Notes for the Agent

### 1. Verify Branch Names Before Use

Branch names can vary between datasets. **Always verify** available branches before coding:

```python
import uproot
f = uproot.open("data.root")
tree = f["Events"]
print("Available branches:", list(tree.keys())[:20])

# Check specific branches
for branch in ["Muon_tightId", "Muon_mediumId", "HLT_IsoMu24", "HLT_IsoMu18"]:
    if branch in tree.keys():
        print(f"  ✓ {branch} exists")
    else:
        print(f"  ✗ {branch} NOT FOUND")
```

### 2. Muon ID Selection

Use ONE of these quality cuts (check which exists):
- **Preferred:** `Muon_tightId` (stricter, cleaner sample)
- **Alternative:** `Muon_mediumId` (more events, slightly more background)

### 3. Trigger Selection

The trigger path may vary. Use OR logic:
```cpp
// Check both triggers
bool passTrigger = HLT_IsoMu24 || HLT_IsoMu18;
```

### 4. Array Access in RDataFrame

Muon branches are arrays (one entry per muon). Use `ROOT::VecOps` for filtering:
```cpp
// Filter muons with pT > 20 GeV
auto goodMuons = Muon_pt[Muon_pt > 20.0f];
```

### 5. Expected Physics

After proper selection, the dimuon invariant mass should show:
- **Z boson peak** at ~91 GeV (dominant feature)
- **J/ψ peak** at ~3.1 GeV (if mass range extends low enough)
- **Υ peaks** at ~9-10 GeV
