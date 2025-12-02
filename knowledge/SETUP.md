# Environment Setup

This document describes how to set up the analysis environment from scratch.
**This is the FIRST step if ROOT or pyhf are not already available.**

## Prerequisites
* Windows 10/11 with WSL 2 installed
* Ubuntu distribution in WSL

## Step 1: Install Miniforge (Conda)

If conda is not installed in WSL:

```bash
# In WSL terminal:
curl -fsSL https://github.com/conda-forge/miniforge/releases/latest/download/Miniforge3-Linux-x86_64.sh -o /tmp/miniforge.sh
bash /tmp/miniforge.sh -b -p ~/miniforge3
rm /tmp/miniforge.sh

# Initialize conda for bash
~/miniforge3/bin/conda init bash
source ~/.bashrc
```

## Step 2: Create the Analysis Environment

```bash
# Create environment with all required packages
conda create -n hep-analysis python=3.11 root cmake pyhf make -c conda-forge -y

# Activate the environment
conda activate hep-analysis
```

**Important:** ROOT is NOT available for Windows native. It MUST be installed via WSL or Linux.

## Step 3: Verify Installation

Run these checks to confirm everything works:

```bash
# Check ROOT
which root
root --version

# Check CMake
cmake --version

# Check Python packages
python -c "import ROOT; print(f'ROOT {ROOT.__version__}')"
python -c "import pyhf; print(f'pyhf {pyhf.__version__}')"
```

Expected output:
- ROOT version 6.x
- CMake version 3.x
- No import errors for Python packages

## Step 4: Verify Data File Access

The data file should be accessible from WSL:

```bash
# If data.root is in Windows at C:\Users\PC\Desktop\temp\desy\
ls -la /mnt/c/Users/PC/Desktop/temp/desy/data.root

# Verify it's a valid ROOT file
python -c "import uproot; f=uproot.open('/mnt/c/Users/PC/Desktop/temp/desy/data.root'); print('OK:', list(f.keys())[:3])"
```

## Troubleshooting

### ROOT not found after conda install
```bash
source ~/miniforge3/etc/profile.d/conda.sh
conda activate hep-analysis
```

### Permission denied errors
```bash
chmod +x ~/miniforge3/bin/*
```

### WSL path conversion
Windows paths must be converted for WSL:
- `C:\Users\PC\file.root` → `/mnt/c/Users/PC/file.root`

### "make: command not found"
```bash
conda install make -c conda-forge -y
```

## Environment Activation (Every Session)

Before running any analysis:
```bash
source ~/miniforge3/etc/profile.d/conda.sh
conda activate hep-analysis
```

Or add to `~/.bashrc` for automatic activation:
```bash
echo 'source ~/miniforge3/etc/profile.d/conda.sh' >> ~/.bashrc
echo 'conda activate hep-analysis' >> ~/.bashrc
```
