import subprocess
import os
import sys
import time
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import io


SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.abspath(os.path.join(SCRIPT_DIR, "..", ".."))
BUILD_DIR = os.path.join(PROJECT_ROOT, "cmake-build-release")

BINARY_PATH = os.path.join(BUILD_DIR, "tests", "benchmark_mycomanager", "compare_vs_libcopp")


def build_benchmark():
    """Configures and builds the C++ target in Release mode."""
    print(f"[*] Starting build process in: {BUILD_DIR}")

    if not os.path.exists(BUILD_DIR):
        print(f"    - Creating build directory: {BUILD_DIR}")
        os.makedirs(BUILD_DIR)

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

    print("    - Building target: compare_vs_libcopp...")

    try:
        # Detect CPU count for parallel build
        cpu_count = os.cpu_count() or 1
    except:
        cpu_count = 1

    build_cmd = [
        "cmake",
        "--build", ".",
        "--target", "compare_vs_libcopp",
        "--config", "Release",
        "-j", str(cpu_count)
    ]

    try:
        subprocess.check_call(build_cmd, cwd=BUILD_DIR)
    except subprocess.CalledProcessError:
        print("[!] Compilation failed. Please check your C++ code errors.")
        sys.exit(1)

    print("[*] Build complete.")


def py_worker(yields):
    for _ in range(yields):
        yield

def bench_python_switch():
    NUM_TASKS = 100
    YIELDS = 10_000

    tasks = [py_worker(YIELDS) for _ in range(NUM_TASKS)]
    start = time.perf_counter_ns()

    while tasks:
        task = tasks.pop(0)
        try:
            next(task)
            tasks.append(task)
        except StopIteration:
            pass

    end = time.perf_counter_ns()
    total_ops = NUM_TASKS * YIELDS
    return (end - start) / (total_ops * 2.0)

def bench_python_lifecycle():
    NUM_TASKS = 50_000
    start = time.perf_counter_ns()

    tasks = [py_worker(0) for _ in range(NUM_TASKS)]
    for t in tasks:
        try:
            next(t)
        except StopIteration:
            pass

    end = time.perf_counter_ns()
    return (end - start) / NUM_TASKS

def run_cpp_benchmark():
    """Runs the compiled C++ binary."""
    target_bin = BINARY_PATH
    if not os.path.exists(target_bin) and os.path.exists(target_bin + ".exe"):
        target_bin += ".exe"

    if not os.path.exists(target_bin):
        print(f"[!] Error: Binary not found at: {target_bin}")
        sys.exit(1)

    print(f"[*] Running C++ benchmark: {target_bin}")
    result = subprocess.run([target_bin], capture_output=True, text=True, check=True)
    return result.stdout

def plot_results(csv_data):
    """Parses CSV and generates a Seaborn plot."""
    try:
        df = pd.read_csv(io.StringIO(csv_data))
    except pd.errors.EmptyDataError:
        print("[!] Error: Benchmark produced no CSV data.")
        sys.exit(1)

    sns.set_theme(style="whitegrid", context="talk")
    plt.figure(figsize=(12, 7))

    palette = {
        "MycoManager": "#3498db",   # Blue
        "LibCopp(Task)": "#e74c3c", # Red
        "Python Gen": "#f1c40f"     # Yellow
    }

    chart = sns.barplot(
        data=df,
        x="Metric",
        y="TimeNS",
        hue="Library",
        palette=palette,
        edgecolor="#2c3e50",
        linewidth=1.5
    )

    chart.set_title("Manager Performance: Myco vs LibCopp vs Python", fontsize=18, pad=20, fontweight='bold')
    chart.set_ylabel("Avg Time per Operation (nanoseconds)\nLower is Better", fontsize=14, labelpad=15)
    chart.set_xlabel("", fontsize=0)
    plt.xticks(fontsize=14, fontweight='bold')
    plt.grid(axis='y', linestyle='--', alpha=0.7)

    for container in chart.containers:
        chart.bar_label(container, fmt='%.1f ns', padding=5, fontweight='bold', fontsize=10)

    out_file = os.path.join(SCRIPT_DIR, "comparison_result.png")
    plt.tight_layout()
    plt.savefig(out_file, dpi=300)
    print(f"[*] Plot saved successfully to: {out_file}")


if __name__ == "__main__":
    try:
        build_benchmark()
        print("-" * 60)

        cpp_output = run_cpp_benchmark()

        print("[*] Running Python benchmarks...")
        py_switch = bench_python_switch()
        py_life = bench_python_lifecycle()


        full_csv = cpp_output.strip() + "\n"
        full_csv += f"Python Gen,Switch,{py_switch}\n"
        full_csv += f"Python Gen,Lifecycle,{py_life}\n"

        print("-" * 60)
        print("Combined Benchmark Data:")
        print(full_csv.strip())
        print("-" * 60)

        plot_results(full_csv)

    except KeyboardInterrupt:
        print("\n[!] Operation interrupted by user.")
        sys.exit(1)
    except Exception as e:
        print(f"[!] An unexpected error occurred: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)