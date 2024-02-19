import argparse
import pandas as pd
import matplotlib.pyplot as plt
import os

# import plotly.express as px


def plot_bandwidth(args):
    input_file = args.input
    output_file = args.output
    routine_name = args.routine
    match args.target:
        case "G3":
            target_hardware = "AWS Graviton3"
        case "G3E":
            target_hardware = "AWS Graviton3E"
        case "A64FX":
            target_hardware = "Fujitsu A64FX"
        case "Grace":
            target_hardware = "NVIDIA Grace"
        case "Rhea1":
            target_hardware = "SiPearl Rhea1"
        case _:
            target_hardware = "unknown hardware"

    # Convert CSV data to DataFrame
    df = pd.read_csv(
        input_file,
        sep="|",
        skipinitialspace=True,
        skiprows=lambda x: x == 1 or x % 3 == 1,
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
    for implementation, group in df.groupby("Implementation"):
        ax.errorbar(
            group["BUF SIZE B"],
            group["BW AVG GiB/s"],
            yerr=group["BW STDEV GiB/s"],
            label=implementation,
            marker="o",
        )

    # Set title and labels
    ax.set_title(f"Average Bandwidth of `{routine_name}`\n", fontsize=24, loc="left")
    ax.text(
        0.02,
        1.02,
        f"on {target_hardware}, "
        f"{'aligned data' if args.is_aligned_alloc else ''}",
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
    # Set ticks on x-axis
    ax.xaxis.set_major_formatter(
        plt.FuncFormatter(lambda x, _: int(x))
    )  # No scientific notation
    # Add grid
    ax.grid(True)

    # Set logarithmic scale if needed
    if args.is_full_sz:
        ax.set_xscale("log")
        if args.target == "G3" or args.target == "G3E":
            l1d = 65536
            ax.axvline(x=l1d, color="r", linestyle="--")
            ax.text(
                l1d,
                2,
                "L1D$\n(64 KiB)",
                color="black",
                fontsize=12,
                ha="center",
                bbox=dict(
                    facecolor="white", edgecolor="red", boxstyle="square,pad=0.5"
                ),
            )
            l2 = 1048576
            ax.axvline(x=l2, color="g", linestyle="--")
            ax.text(
                l2,
                2,
                "L2$\n(1 MiB)",
                color="black",
                fontsize=12,
                ha="center",
                bbox=dict(facecolor="white", edgecolor="g", boxstyle="square,pad=0.5"),
            )
            l3 = 33554432
            ax.axvline(x=l3, color="b", linestyle="--")
            ax.text(
                l3,
                2,
                "L3$\n(32 MiB)",
                color="black",
                fontsize=12,
                ha="center",
                bbox=dict(facecolor="white", edgecolor="b", boxstyle="square,pad=0.5"),
            )

    # Save the plot
    plt.savefig(output_file)


# fig = px.line(
#     df,
#     x="BUF SIZE B",
#     y="BW AVG GiB/s",
#     error_y="BW STDEV GiB/s",
#     color="Implementation",
#     markers=True,
#     category_orders={"BUF SIZE B": df["BUF SIZE B"].unique()},
#     title=f"<b>Average bandwidth of `{routine_name}` implementations</b>"
#     f"<br><sup>on {target_hardware}</sup>"
#     f"{'(aligned allocations)' if args.is_aligned_alloc else ''}",
# )

# # Set the resolution of the figure
# fig.update_layout(
#     font=dict(size=20),
#     width=1400,
#     height=900,
#     yaxis=dict(tickmode="linear", dtick=1 if df["BW AVG GiB/s"].max() <= 10 else 2),
#     xaxis_title="<b>Buffer size (Bytes)</b>",
#     yaxis_title="<b>Average bandwidth (GiB/s)</b>",
#     # legend=dict(
#     #     font=dict(size=24),
#     #     title="<b>Implementation</b>",
#     #     yanchor="bottom",
#     #     y=0.01,
#     #     xanchor="right",
#     #     x=0.99,
#     #     borderwidth=8,
#     #     bordercolor='white'
#     # )
# )

# if args.is_log_x:
#     fig.update_xaxes(type="log")

# # fig.add_annotation(
# #     font=dict(size=22),
# #     x=0.45,
# #     y=0.03,
# #     text="<b>Higher is Better</b>",
# #     bgcolor='white',
# #     borderwidth=8,
# #     xanchor='left',
# #     xref="paper",
# #     yref="paper"
# # )

# # Save the plot
# fig.write_image(output_file)


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
    plot_bandwidth(args)
