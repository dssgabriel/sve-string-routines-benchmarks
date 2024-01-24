import argparse
import pandas as pd
import plotly.express as px
import os


def plot_bandwidth(input_file, output_file):
    # Retrieve routine name from the input file name
    routine_name = os.path.splitext(os.path.basename(input_file))[0]
    
    # Convert CSV data to DataFrame
    df = pd.read_csv(input_file, sep='|', skipinitialspace=True, skiprows=lambda x: x == 1 or x % 3 == 1)
    # Remove whitespace from column entries
    df = df.apply(lambda x: x.str.strip() if x.dtype == 'object' else x)
    # Remove whitespace from column names
    df.columns = df.columns.str.strip()

    # Extract relevant columns
    df = df[['ROUTINE IMPLEMENTATION', 'BUF SIZE', 'BW AVG GiB/s', 'BW STDEV GiB/s']]

    # Create a new column for implementation type
    df['Implementation'] = df['ROUTINE IMPLEMENTATION'].apply(lambda x: f'Current' if 'Current' in x else f'New')
    # Rename BW column
    df['Bandwidth (GiB/s)'] = df['BW AVG GiB/s']

    # Plot the grouped bar chart
    fig = px.bar(
        df,
        x='BUF SIZE',
        y='BW AVG GiB/s',
        error_y='BW STDEV GiB/s',
        color='Implementation',
        barmode='group',
        # text_auto=True,
        category_orders={'BUF SIZE': df['BUF SIZE'].unique()},
        title=f'<b>Comparative bandwidth performance of `{routine_name}_aarch64_sve` implementations</b>'
            '<br><sup>on AWS Graviton3E (hpc7g.16xlarge)</sup>'
    )

    # Set the resolution of the figure
    fig.update_layout(
        font=dict(size=20),
        width=1280, height=960,
        yaxis=dict(tickmode='linear', dtick=1 if df['BW AVG GiB/s'].max() <= 10 else 2),
        xaxis_title="<b>String buffer size</b>",
        yaxis_title="<b>Bandwidth (in GiB/s)</b>",
        legend=dict(
            font=dict(size=24),
            title="<b>Implementation</b>",
            yanchor="bottom",
            y=0.01,
            xanchor="left",
            x=0.01,
            borderwidth=8,
            bordercolor='white'
        )
    )

    fig.add_annotation(
        font=dict(size=22),
        x=0.424,
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
    parser.add_argument("-i", "--input", help="Input CSV file name", required=True)
    parser.add_argument("-o", "--output", help="Output file name for the plot", required=True)
    args = parser.parse_args()

    plot_bandwidth(args.input, args.output)
