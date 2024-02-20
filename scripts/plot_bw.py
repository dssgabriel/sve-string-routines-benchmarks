import argparse
import pandas as pd
import matplotlib.pyplot as plt
import os


def plot_bandwidth(args):
    input_file = args.input
    output_file = args.output
    routine_name = args.routine
    match args.target:
        case "G3":
            target_hardware = "AWS Graviton3"
            l1d = 65536
            l2 = 1048576
            l3 = 33554432
        case "G3E":
            target_hardware = "AWS Graviton3E"
            l1d = 65536
            l2 = 1048576
            l3 = 33554432
        case "A64FX":
            target_hardware = "Fujitsu A64FX"
            l1d = 65536
            l2 = 838860
            l3 = None
        case "Grace":
            target_hardware = "NVIDIA Grace"
            l1d = 65536
            l2 = 1048576
            l3 = 245366784
        case "Rhea1":
            target_hardware = "SiPearl Rhea1"
            l1d = 65536
            l2 = 1048576
            l3 = 33554432
        case _:
            target_hardware = "unknown hardware"

    # Convert CSV data to DataFrame
    df = pd.read_csv(
        input_file,
        sep="|",
        skipinitialspace=True,
    )
    # Remove whitespace from column entries
    df = df.apply(lambda x: x.str.strip() if x.dtype == "object" else x)
    # Remove whitespace from column names
    df.columns = df.columns.str.strip()

    # Extract relevant columns
    df = df[["ROUTINE IMPLEMENTATION", "BUF SIZE B", "BW AVG GiB/s", "BW STDEV GiB/s"]]

    df["Implementation"] = df["ROUTINE IMPLEMENTATION"]
    df["Average bandwidth (GiB/s)"] = df["BW AVG GiB/s"]

    # Create figure and axis objects
    fig, ax = plt.subplots(figsize=(14, 9))

    # Plot the data
    if args.is_full_sz:
        marker = "-o"
    else:
        marker = "-"
    for implementation, group in df.groupby("Implementation"):
        ax.plot(group["BUF SIZE B"], group["BW AVG GiB/s"], marker, label=implementation)
        ax.fill_between(
            group["BUF SIZE B"],
            group["BW AVG GiB/s"] - group["BW STDEV GiB/s"],
            group["BW AVG GiB/s"] + group["BW STDEV GiB/s"],
            alpha=0.3
        )
        # ax.errorbar(
        #     group["BUF SIZE B"],
        #     group["BW AVG GiB/s"],
        #     yerr=group["BW STDEV GiB/s"],
        #     marker="o",
        # )

    # Set title and labels
    ax.set_title(f"Average Bandwidth of `{routine_name}`\n", fontsize=24, loc="left")
    ax.text(
        0.01,
        1.01,
        f"on {target_hardware}"
        f"{', aligned data' if args.is_aligned_alloc else ''}"
        " - Higher is Better",
        ha="left",
        va="bottom",
        fontsize=12,
        transform=ax.transAxes,
        color="#444444",
    )
    ax.set_xlabel("Buffer size (Bytes)")
    ax.set_ylabel("Average bandwidth (GiB/s)")

    # Set legend
    ax.legend(title="Implementation")

    # Set y-axis limit to start from zero
    ax.set_ylim(bottom=0)
    # Add grid
    ax.grid(True)

    if args.is_full_sz:
        # Set logarithmic scale if needed
        ax.set_xscale("log")
        # Add cache sizes where possible
        if target_hardware != "unknown target":
            ax.axvline(x=l1d, color="r", linestyle="--")
            ax.text(
                l1d,
                2,
                f"L1D$\n({l1d / 1024:.0f} KiB)",
                color="black",
                fontsize=12,
                ha="center",
                bbox=dict(
                    facecolor="white", edgecolor="red", boxstyle="square,pad=0.5"
                ),
            )
            ax.axvline(x=l2, color="g", linestyle="--")
            ax.text(
                l2,
                2,
                f"L2$\n({l2 / 1048576:.0f} MiB)",
                color="black",
                fontsize=12,
                ha="center",
                bbox=dict(facecolor="white", edgecolor="g", boxstyle="square,pad=0.5"),
            )
            if l3 is not None:
                ax.axvline(x=l3, color="b", linestyle="--")
                ax.text(
                    l3,
                    2,
                    f"L3$\n({l3 / 1048576:.0f} MiB)",
                    color="black",
                    fontsize=12,
                    ha="center",
                    bbox=dict(facecolor="white", edgecolor="b", boxstyle="square,pad=0.5"),
                )

    # Save the plot
    plt.savefig(output_file)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Generate a bar plot of bandwidth over buffer sizes."
    )
    parser.add_argument(
        "-i", "--input", dest="input", help="Input CSV file name", required=True
    )
    parser.add_argument(
        "-o", "--output", dest="output", help="Output file name for the plot"
    )
    parser.add_argument(
        "-r",
        "--routine",
        dest="routine",
        choices=[
            "memcpy",
            "strcpy",
            "strncpy",
            "memcmp",
            "strcmp",
            "strncmp",
            "strchr",
            "strrchr",
            "strlen",
            "strnlen",
        ],
        help="Routine name",
        required=True,
    )
    parser.add_argument(
        "-t",
        "--target",
        dest="target",
        choices=["G3", "G3E", "A64FX", "Grace", "Rhea1"],
        help="Target hardware",
    )
    parser.add_argument(
        "--aligned-alloc",
        dest="is_aligned_alloc",
        help="Specify aligned allocations",
        action="store_true",
    )
    parser.add_argument(
        "--full-sizes",
        dest="is_full_sz",
        help="Specify full sizes run",
        action="store_true",
    )
    args = parser.parse_args()

    if not args.output:
        os.path.join(os.getcwd(), os.listdir(os.getcwd())[0])
        args.output = (
            "results/plots/"
            + os.path.basename(os.path.splitext(args.input)[0])
            + ".png"
        )

    with open(args.input, "r+") as f:
        d = f.readlines()
        f.seek(0)
        for l in d:
            if not l.startswith("--"):
                f.write(l)
        f.truncate()
    plot_bandwidth(args)
