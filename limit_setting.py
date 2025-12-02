#!/usr/bin/env python3
"""
Limit Setting with pyhf

This script demonstrates statistical limit setting using pyhf.
It creates a simple single-bin counting experiment model with:
- Observed events
- Expected background (with systematic uncertainty)
- Expected signal

The CLs method is used to calculate exclusion limits.
"""

import json
import pyhf


def create_model(signal: float, background: float, bkg_uncertainty: float) -> pyhf.Model:
    """
    Create a pyhf model for a single-bin counting experiment.

    Args:
        signal: Expected number of signal events
        background: Expected number of background events
        bkg_uncertainty: Absolute uncertainty on background

    Returns:
        pyhf Model object
    """
    # Build the model specification
    spec = {
        "channels": [
            {
                "name": "singlechannel",
                "samples": [
                    {
                        "name": "signal",
                        "data": [signal],
                        "modifiers": [
                            {
                                "name": "mu",
                                "type": "normfactor",
                                "data": None
                            }
                        ]
                    },
                    {
                        "name": "background",
                        "data": [background],
                        "modifiers": [
                            {
                                "name": "bkg_uncertainty",
                                "type": "staterror",
                                "data": [bkg_uncertainty]
                            }
                        ]
                    }
                ]
            }
        ]
    }

    return pyhf.Model(spec)


def calculate_cls_limit(model: pyhf.Model, observations: list, poi_test: float = 1.0) -> dict:
    """
    Calculate CLs values for hypothesis testing.

    Args:
        model: pyhf Model
        observations: Observed event counts
        poi_test: Parameter of interest value to test (signal strength)

    Returns:
        Dictionary with CLs values
    """
    # Get initial parameters
    init_pars = model.config.suggested_init()
    par_bounds = model.config.suggested_bounds()

    # Calculate CLs using the asymptotic calculator
    CLs_obs, CLs_exp = pyhf.infer.hypotest(
        poi_test,
        observations + model.config.auxdata,
        model,
        init_pars,
        par_bounds,
        return_expected_set=True
    )

    return {
        "CLs_obs": float(CLs_obs),
        "CLs_exp": [float(x) for x in CLs_exp],  # [-2σ, -1σ, median, +1σ, +2σ]
    }


def calculate_upper_limit(model: pyhf.Model, observations: list) -> dict:
    """
    Calculate the 95% CL upper limit on the signal strength.

    Args:
        model: pyhf Model
        observations: Observed event counts

    Returns:
        Dictionary with upper limit values
    """
    import numpy as np

    # Scan over mu values to find the 95% CL upper limit
    poi_values = np.linspace(0.0, 10.0, 21)

    obs_limit, exp_limits = pyhf.infer.intervals.upper_limits.upper_limit(
        observations + model.config.auxdata,
        model,
        poi_values,
        level=0.05,  # 95% CL
    )

    return {
        "observed_upper_limit": float(obs_limit),
        "expected_upper_limit": float(exp_limits[2]),  # median expected
    }


def main():
    """Main function to run the limit setting analysis."""
    print("=" * 60)
    print("pyhf Limit Setting Analysis")
    print("=" * 60)

    # Define the model parameters
    observed_events = 100
    expected_background = 100.0
    background_uncertainty = 10.0  # +/- 10 events (10% uncertainty)
    expected_signal = 10.0

    print(f"\nModel Configuration:")
    print(f"  Observed events:      {observed_events}")
    print(f"  Expected background:  {expected_background} +/- {background_uncertainty}")
    print(f"  Expected signal:      {expected_signal}")

    # Create the model
    print("\nCreating pyhf model...")
    model = create_model(
        signal=expected_signal,
        background=expected_background,
        bkg_uncertainty=background_uncertainty
    )

    observations = [observed_events]

    # Print model info
    print(f"  Number of channels: {len(model.config.channels)}")
    print(f"  Number of parameters: {len(model.config.parameters)}")
    print(f"  Parameters: {model.config.parameters}")

    # Calculate CLs at mu=1 (nominal signal strength)
    print("\n" + "-" * 60)
    print("Hypothesis Test (mu = 1.0)")
    print("-" * 60)

    cls_result = calculate_cls_limit(model, observations, poi_test=1.0)

    print(f"\n  CLs observed:        {cls_result['CLs_obs']:.4f}")
    print(f"\n  CLs expected band:")
    print(f"    -2 sigma:          {cls_result['CLs_exp'][0]:.4f}")
    print(f"    -1 sigma:          {cls_result['CLs_exp'][1]:.4f}")
    print(f"    median:            {cls_result['CLs_exp'][2]:.4f}")
    print(f"    +1 sigma:          {cls_result['CLs_exp'][3]:.4f}")
    print(f"    +2 sigma:          {cls_result['CLs_exp'][4]:.4f}")

    # Interpret the result
    alpha = 0.05  # 95% CL
    if cls_result['CLs_obs'] < alpha:
        print(f"\n  Result: Signal hypothesis EXCLUDED at 95% CL")
        print(f"          (CLs = {cls_result['CLs_obs']:.4f} < {alpha})")
    else:
        print(f"\n  Result: Signal hypothesis NOT excluded at 95% CL")
        print(f"          (CLs = {cls_result['CLs_obs']:.4f} >= {alpha})")

    # Calculate upper limit
    print("\n" + "-" * 60)
    print("Upper Limit Calculation (95% CL)")
    print("-" * 60)

    try:
        ul_result = calculate_upper_limit(model, observations)
        print(f"\n  Observed 95% CL upper limit on mu: {ul_result['observed_upper_limit']:.3f}")
        print(f"\n  This means: signal cross-section is excluded at {ul_result['observed_upper_limit']:.3f}x")
        print(f"              the nominal signal hypothesis at 95% CL")
    except Exception as e:
        print(f"\n  Could not calculate upper limit: {e}")
        print("  (This can happen if the limit is outside the scanned range)")

    print("\n" + "=" * 60)
    print("Analysis complete!")
    print("=" * 60)

    # Return results for programmatic use
    return {
        "observed": observed_events,
        "background": expected_background,
        "background_uncertainty": background_uncertainty,
        "signal": expected_signal,
        "CLs": cls_result,
    }


if __name__ == "__main__":
    results = main()
