import argparse
import pandas as pd
import plotly.express as px
import os

def plot_bandwidth(input_file, output_file):
    # Retrieve routine name from the input file name
    routine_name = os.path.splitext(os.path.basename(input_file))[0]
    
    # Convert CSV data to DataFrame
    df = pd.read_csv(input_file, sep='|', skipinitialspace=True)

    # Extract relevant columns
    df = df[['BW GiB/s', 'BUF SIZE', 'ROUTINE IMPLEM']]

    # Create a new column for implementation type
    df['Implementation'] = df['ROUTINE IMPLEM'].apply(lambda x: f'Current' if 'Current' in x else f'New')
    df['Bandwidth (GiB/s)'] = df['BW GiB/s']

    # Plot the grouped bar chart
    fig = px.bar(
        df,
        x='BUF SIZE',
        y='Bandwidth (GiB/s)',
        color='Implementation',
        barmode='group',
        category_orders={'BUF SIZE': df['BUF SIZE'].unique()},
        labels={'BUF SIZE': 'String Buffer Size'},
        text_auto=True,
        title=f"<b>Comparative bandwidth performance of `{routine_name}_aarch64_sve` implementations</b>"
    )

    # Set the resolution of the figure
    fig.update_layout(width=1024, height=800, yaxis=dict(tickmode='linear', dtick=2.5))

    # Save the plot
    fig.write_image(output_file)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate a bar plot of bandwidth over buffer sizes.")
    parser.add_argument("-i", "--input", help="Input CSV file name", required=True)
    parser.add_argument("-o", "--output", help="Output PDF file name for the plot", required=True)
    args = parser.parse_args()

    plot_bandwidth(args.input, args.output)
