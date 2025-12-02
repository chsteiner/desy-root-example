# DESY ROOT Example: Agentic Coding with Promptotyping

A workshop showcase demonstrating how to use **Promptotyping** as a context engineering strategy for AI-assisted software development in High Energy Physics.

## About This Workshop

This repository is part of a "Generative AI & Agentic Coding" workshop at DESY. The goal is to demonstrate how structured documentation (Promptotyping Documents) can guide an AI coding agent to build a complete particle physics analysis from scratch.

**The Challenge:** Given only the `data.root` file and the knowledge documents, can an AI agent produce a working Z boson analysis?

## What is Promptotyping?

**Promptotyping** (Prompt + Prototype) is a structured prompt engineering methodology developed by [Digital Humanities Craft](https://dhcraft.org). It creates systematic requirement documents that enable LLM-assisted software development through **Context Compression** - reducing complex information to essential, precisely-worded tokens for focused AI attention.

### The Three Central Questions

| Category | Question | Purpose |
|----------|----------|---------|
| **WHAT?** | What are we building and why? | Context, motivation, requirements |
| **USING WHAT?** | What data/resources do we use? | Data schemas, inputs, constraints |
| **HOW?** | How do we implement it? | Instructions, rules, design decisions |

### Promptotyping Documents in This Repository

```
knowledge/
├── SETUP.md          # HOW?  - Environment bootstrap
├── DATA.md           # USING WHAT? - Data schema & branch names
├── REQUIREMENTS.md   # WHAT? - Functional requirements
├── RULES.md          # HOW?  - Operational constraints & gotchas
└── INSTRUCTIONS.md   # HOW?  - Step-by-step implementation plan
```

### Key Principles

1. **Context Compression** - Distill complex domain knowledge into essential tokens
2. **Context Amplification** - Emphasize relevant information, reduce noise
3. **Expert-in-the-Loop** - Domain experts validate outputs iteratively
4. **Structured Documentation** - Markdown format for clear, parseable context

> *"Promptotyping forces us to think through problems precisely - down to the individual token."*
> — Christopher Pollin, Digital Humanities Craft

---

## The Physics Task

Reconstruct the **Z boson** in the dimuon decay channel (Z → μ⁺μ⁻) using real CMS Open Data from 2016.

### Scientific Objective

1. Select events with exactly two opposite-sign, high-quality muons
2. Calculate the dimuon invariant mass
3. Observe the Z boson resonance peak at ~91 GeV
4. Perform statistical limit setting (demonstration)

### Expected Result

The dimuon invariant mass spectrum should show a clear **Z boson peak at ~91 GeV**.

---

## Data

**Download the data file:**
- **URL:** https://cernbox.cern.ch/s/MgWQqwrxwuiRdyQ
- **Filename:** `data.root`
- **Size:** ~2.3 GB
- **Format:** CMS NanoAOD (real collision data)

Place `data.root` in the repository root directory.

---

## Repository Structure

```
desy-root-example/
├── README.md              # This file (Workshop overview + Promptotyping explanation)
├── LICENSE                # CC BY 4.0
├── knowledge/             # Promptotyping Documents (input for AI agent)
│   ├── SETUP.md           # Environment setup instructions
│   ├── DATA.md            # Data schema documentation
│   ├── REQUIREMENTS.md    # Project requirements
│   ├── RULES.md           # Operational rules & constraints
│   └── INSTRUCTIONS.md    # Implementation plan
└── data.root              # Input data (download separately)
```

### Solution Branch

The `solution` branch contains a complete working implementation:
- C++ analysis with ROOT RDataFrame
- Python limit setting with pyhf
- CMakeLists.txt build system

```bash
git checkout solution
```

---

## Workshop Flow

### For Participants (Using an AI Agent)

1. **Clone this repository**
   ```bash
   git clone https://github.com/your-org/desy-root-example.git
   cd desy-root-example
   ```

2. **Download the data**
   - Get `data.root` from the link above
   - Place it in the repository root

3. **Feed the knowledge documents to your AI agent**
   - Start with `knowledge/INSTRUCTIONS.md`
   - The agent should read all documents in `knowledge/`
   - Let the agent implement the analysis

4. **Validate the output**
   - Does the dimuon mass plot show a peak at ~91 GeV?
   - Does the cutflow make physical sense?

### For Instructors

The `solution` branch provides a reference implementation that was generated using Claude Code with these same Promptotyping documents.

---

## Technical Requirements

- **OS:** Windows with WSL 2 (Ubuntu) or native Linux/macOS
- **Package Manager:** Conda (Miniforge recommended)
- **Frameworks:** ROOT 6.x, CMake, pyhf

See `knowledge/SETUP.md` for detailed environment setup.

---

## Quick Start (Solution)

```bash
# Switch to solution branch
git checkout solution

# Set up environment (in WSL)
source ~/miniforge3/etc/profile.d/conda.sh
conda activate hep-analysis  # or create it per SETUP.md

# Build
mkdir build && cd build
cmake .. && make -j4

# Run (quick test)
./dimuon_analysis -i ../data.root -n 100000

# Run Python limit setting
cd .. && python limit_setting.py
```

---

## Credits & License

### Promptotyping Methodology
Developed by **Christian Steiner** and **Christopher Pollin** at [Digital Humanities Craft OG](https://dhcraft.org).

Based on the blog post: [Promptotyping: Von der Idee zur Anwendung](https://dhcraft.org/excellence/blog/Promptotyping)

### Workshop Materials
Created for DESY "Generative AI & Agentic Coding" workshop.

### License
This work is licensed under [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/).

You are free to:
- **Share** — copy and redistribute the material
- **Adapt** — remix, transform, and build upon the material

Under the following terms:
- **Attribution** — You must give appropriate credit to Digital Humanities Craft OG and the original authors.

---

## References

- [CMS Open Data Portal](http://opendata.cern.ch/)
- [ROOT Data Analysis Framework](https://root.cern/)
- [pyhf - HistFactory in Python](https://pyhf.readthedocs.io/)
- [Promptotyping Methodology (DHCraft)](https://dhcraft.org/excellence/blog/Promptotyping)
