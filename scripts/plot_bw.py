import argparse
import pandas as pd
import plotly.express as px
import os


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
    df = pd.read_csv(input_file, sep='|', skipinitialspace=True, skiprows=lambda x: x == 1 or x % 4 == 1)
    # Remove whitespace from column entries
    df = df.apply(lambda x: x.str.strip() if x.dtype == 'object' else x)
    # Remove whitespace from column names
    df.columns = df.columns.str.strip()

    # Extract relevant columns
    df = df[['ROUTINE IMPLEMENTATION', 'BUF SIZE B', 'BW AVG GiB/s', 'BW STDEV GiB/s']]

    # Create a new column for implementation type
    df['Implementation'] = df['ROUTINE IMPLEMENTATION'].apply(lambda x: "LI-PaRAD/EMOPASS" if "New" in x else x)
    # Rename BW column
    df['Average bandwidth (GiB/s)'] = df['BW AVG GiB/s']

    fig = px.line(
        df,
        x='BUF SIZE B',
        y='BW AVG GiB/s',
        error_y='BW STDEV GiB/s',
        color='Implementation',
        markers=True,
        category_orders={'BUF SIZE B': df['BUF SIZE B'].unique()},
        title=f'<b>Comparative average bandwidth performance of `{routine_name}` implementations</b>'
            f'<br><sup>on {target_hardware}</sup>'
    )

    # Set the resolution of the figure
    fig.update_layout(
        font=dict(size=20),
        width=1400, height=900,
        yaxis=dict(tickmode='linear', dtick=1 if df['BW AVG GiB/s'].max() <= 10 else 2),
        xaxis_title="<b>Buffer size (Bytes)</b>",
        yaxis_title="<b>Average bandwidth (GiB/s)</b>",
        legend=dict(
            font=dict(size=24),
            title="<b>Implementation</b>",
            yanchor="bottom",
            y=0.01,
            xanchor="right",
            x=0.99,
            borderwidth=8,
            bordercolor='white'
        )
    )

    fig.add_annotation(
        font=dict(size=22),
        x=0.45,
        y=0.03,
        text="<b>Higher is Better</b>",
        bgcolor='white',
        borderwidth=8,
        xanchor='left',
        xref="paper",
        yref="paper"
    )

    # Save the plot
    fig.write_image(output_file)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate a bar plot of bandwidth over buffer sizes.")
    parser.add_argument("-i", dest="input", help="Input CSV file name", required=True)
    parser.add_argument("-o", dest="output", help="Output file name for the plot")
    parser.add_argument("-r", dest="routine", choices=["memcpy", "strcpy", "strncpy", "memcmp", "strcmp", "strncmp", "strchr", "strrchr", "strlen", "strnlen"], help="Routine name", required=True)
    # parser.add_argument("-c", dest="implem", choices=["libc", "aor"], help="Implementation being compared to", required=True)
    parser.add_argument("-t", dest="target", choices=["G3", "G3E", "A64FX", "Grace", "Rhea1"], help="Target hardware")
    args = parser.parse_args()

    if not args.output:
        os.path.join(os.getcwd(), os.listdir(os.getcwd())[0])
        args.output = "results/plots/" + os.path.basename(os.path.splitext(args.input)[0]) + ".png"
    plot_bandwidth(args)
