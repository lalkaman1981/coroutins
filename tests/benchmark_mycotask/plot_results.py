import subprocess
import os
import sys
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import io
import shutil

# --- Paths ---
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
# Project root is two levels up from tests/benchmark_mycotask/
PROJECT_ROOT = os.path.abspath(os.path.join(SCRIPT_DIR, "..", ".."))
BUILD_DIR = os.path.join(PROJECT_ROOT, "cmake-build-release")
# Path to the final executable
BINARY_PATH = os.path.join(BUILD_DIR, "tests", "benchmark_mycotask", "bench_compare")

def build_benchmark():
    """Configures and builds the benchmark in Release mode."""
    print(f"[*] Starting build process in: {BUILD_DIR}")

    # 1. Create Build Directory
    if not os.path.exists(BUILD_DIR):
        print(f"    - Creating build directory: {BUILD_DIR}")
        os.makedirs(BUILD_DIR)

    # 2. Configure (CMake)
    print("    - Configuring CMake (Release Mode)...")
    cmake_config_cmd = [
        "cmake",
        "-DCMAKE_BUILD_TYPE=Release",
        PROJECT_ROOT
    ]

    try:
        subprocess.check_call(cmake_config_cmd, cwd=BUILD_DIR, stdout=subprocess.DEVNULL)
    except subprocess.CalledProcessError:
        print("[!] CMake configuration failed.")
        sys.exit(1)

    # 3. Build the specific target (bench_compare)
    print("    - Building target: bench_compare...")

    # Determine CPU count for parallel build
    try:
        cpu_count = len(os.sched_getaffinity(0))
    except AttributeError:
        cpu_count = os.cpu_count() or 1

    build_cmd = [
        "cmake",
        "--build", ".",
        "--target", "bench_compare",
        "--config", "Release",
        "-j", str(cpu_count)
    ]

    try:
        subprocess.check_call(build_cmd, cwd=BUILD_DIR) # Show build output
    except subprocess.CalledProcessError:
        print("[!] Compilation failed.")
        sys.exit(1)

    print("[*] Build complete.")

def run_benchmark():
    """Runs the built benchmark binary."""
    # FIX: global declaration must be at the top
    global BINARY_PATH

    if not os.path.exists(BINARY_PATH) and not os.path.exists(BINARY_PATH + ".exe"):
        # Try to find it if path structure is different (e.g. Windows Release folder)
        alternative_path = os.path.join(BUILD_DIR, "tests", "benchmark_mycotask", "Release", "bench_compare.exe")
        if os.path.exists(alternative_path):
            BINARY_PATH = alternative_path
        else:
            print(f"[!] Error: Binary not found at expected paths:\n    1. {BINARY_PATH}\n    2. {alternative_path}")
            sys.exit(1)

    print(f"[*] Running benchmark: {BINARY_PATH}")
    # Run binary and capture stdout
    result = subprocess.run([BINARY_PATH], capture_output=True, text=True, check=True)
    return result.stdout

def plot(csv_data):
    """Parses CSV output and generates a plot."""
    try:
        df = pd.read_csv(io.StringIO(csv_data))
    except pd.errors.EmptyDataError:
        print("[!] Error: Benchmark produced no CSV data.")
        sys.exit(1)

    # Set Visual Theme
    sns.set_theme(style="whitegrid", context="talk")
    plt.figure(figsize=(12, 7))

    # Define Colors
    palette = {"MycoTask": "#3498db", "LibCopp": "#e74c3c"}

    # Create Bar Plot
    chart = sns.barplot(
        data=df,
        x="Metric",
        y="TimeNS",
        hue="Library",
        palette=palette,
        edgecolor="#2c3e50",
        linewidth=1.5
    )

    # Styling
    chart.set_title("Performance Comparison: MycoTask vs LibCopp", fontsize=18, pad=20, fontweight='bold')
    chart.set_ylabel("Avg Time per Operation (nanoseconds)\nLower is Better", fontsize=14, labelpad=15)
    chart.set_xlabel("", fontsize=0) # Remove X label
    plt.xticks(fontsize=14, fontweight='bold')
    plt.yticks(fontsize=12)
    plt.grid(axis='y', linestyle='--', alpha=0.7)

    # Add data labels on top of bars
    for container in chart.containers:
        chart.bar_label(container, fmt='%.1f ns', padding=5, fontweight='bold', fontsize=12)

    # Save the plot
    out_file = os.path.join(SCRIPT_DIR, "comparison_result.png")
    plt.tight_layout()
    plt.savefig(out_file, dpi=300)
    print(f"[*] Plot saved successfully to: {out_file}")

if __name__ == "__main__":
    try:
        # 1. Build
        build_benchmark()
        print("-" * 60)

        # 2. Run
        csv_output = run_benchmark()
        print("-" * 60)
        print("Raw Benchmark Data:")
        print(csv_output.strip())
        print("-" * 60)

        # 3. Plot
        plot(csv_output)

    except KeyboardInterrupt:
        print("\n[!] Operation interrupted by user.")
        sys.exit(1)
    except Exception as e:
        print(f"[!] An unexpected error occurred: {e}")
        sys.exit(1)