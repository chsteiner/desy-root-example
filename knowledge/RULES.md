# Operational Rules & Constraints

## 1. Environment (WSL on Windows)

### Platform Rules
* We are running on **Linux** (Ubuntu via WSL 2)
* **Do NOT** use Windows-specific commands:
  - No `.exe` extensions
  - No `cls` (use `clear`)
  - No backslashes in paths
* Use forward slashes `/` for all paths
* **Compilation:** Use `cmake` and `make` (not MSBuild or Visual Studio)

### Path Conversion
When accessing Windows files from WSL:
```
Windows: C:\Users\PC\Desktop\temp\desy\data.root
WSL:     /mnt/c/Users/PC/Desktop/temp/desy/data.root
```

---

## 2. Dependency Management

### Pre-installed (in conda environment)
* ROOT (with RDataFrame)
* CMake
* make
* pyhf
* Python 3.11

### Installation Rules
* **Do NOT** use `apt-get install` unless explicitly requested
* **Do NOT** use `pip install` for packages available via conda
* If a package is missing, install via conda:
  ```bash
  conda install <package> -c conda-forge -y
  ```

### CMake Configuration
* Use `find_package(ROOT REQUIRED ...)` to locate ROOT
* ROOT provides `${ROOT_USE_FILE}` - include it for proper setup
* Link targets with `ROOT::ROOTDataFrame`, `ROOT::ROOTVecOps`, etc.

---

## 3. Coding Standards (C++)

### Language Standard
* **C++17** minimum (ROOT 6.36+ may default to C++20)
* Set explicitly in CMakeLists.txt:
  ```cmake
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  ```

### Style Guidelines
* Use clear, descriptive variable names
* Add comments explaining **physics logic** (not obvious code)
* Prefer `auto` for complex ROOT types
* Use `std::optional` for optional parameters

### Memory Management
* RDataFrame handles memory automatically - don't manually delete
* Use smart pointers (`std::unique_ptr`, `std::shared_ptr`) if needed
* Avoid raw `new`/`delete`

---

## 4. Known RDataFrame Issues

### CRITICAL: Range() + ImplicitMT() Incompatibility

**Problem:** `ROOT::EnableImplicitMT()` and `RDataFrame::Range()` cannot be used together.

**Solution:** Disable multithreading when using `-n` flag:
```cpp
if (maxEvents.has_value()) {
    // Single-threaded mode for Range()
    std::cout << "Processing " << maxEvents.value() << " events (single-threaded)" << std::endl;
} else {
    ROOT::EnableImplicitMT();  // Only enable MT for full dataset
}
```

### Branch Existence Checking
Before using branches in `Define()` or `Filter()`, verify they exist:
```cpp
auto colNames = df.GetColumnNames();
bool hasTightId = std::find(colNames.begin(), colNames.end(), "Muon_tightId") != colNames.end();
```

### Cutflow Reporting
Always use named filters for debugging:
```cpp
df.Filter("nMuon == 2", "Exactly 2 muons")  // Named filter
  .Filter(myLambda, {"col"}, "My custom cut")  // Also named
```
Then print cutflow with:
```cpp
auto report = df.Report();
report->Print();
```

---

## 5. Error Handling

### File Not Found
```cpp
if (!std::filesystem::exists(inputFile)) {
    std::cerr << "Error: Input file not found: " << inputFile << std::endl;
    return 1;
}
```

### Missing Branches
* Warn the user but don't crash
* Use `Muon_tightId` for muon identification

### ROOT Warnings
* `Warning in <TClass::Init>: no dictionary for class edm::...` is **normal** for NanoAOD
* These can be safely ignored

---

## 6. Output Guidelines

### Console Output
* Print clear progress messages
* Show cutflow statistics
* Report number of selected events

### File Output
* PNG plots: 800x600 pixels, clear axis labels
* ROOT file: Include histogram titles and axis labels
* Use descriptive filenames: `dimuon_mass.png`, not `plot1.png`

---

## 7. Git & Version Control

### Do NOT commit:
* `build/` directory
* `*.root` data files (too large)
* `*.png` output files
* `__pycache__/`

### Suggested .gitignore:
```
build/
*.root
*.png
__pycache__/
*.pyc
```
