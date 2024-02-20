import argparse
import pandas as pd

def merge_files(input_file1, input_file2, output_file):
    # Read the first file
    df1 = pd.read_csv(input_file1, sep="|", skipinitialspace=True)
    # Remove whitespace from column entries
    df1 = df1.apply(lambda x: x.str.strip() if x.dtype == "object" else x)
    # Remove whitespace from column names
    df1.columns = df1.columns.str.strip()
    df1.drop(columns="SPEEDUP", inplace=True)

    # Read the second file
    df2 = pd.read_csv(input_file2, sep="|", skipinitialspace=True)
    # Remove whitespace from column entries
    df2 = df2.apply(lambda x: x.str.strip() if x.dtype == "object" else x)
    # Remove whitespace from column names
    df2.columns = df2.columns.str.strip()
    df2.drop(columns="SPEEDUP", inplace=True)

    # Merge the two dataframes on 'BUF SIZE B' and 'ROUTINE IMPLEMENTATION'
    merged_df = pd.merge(df1, df2, on=['BUF SIZE B', 'ROUTINE IMPLEMENTATION'], how='outer')
    # Copy values from overlapping columns if one column has NaN
    for column in df1.columns[2:]:
        if column in df2.columns[2:]:
            merged_df[column] = merged_df[column + '_x'].combine_first(merged_df[column + '_y'])
            merged_df.drop(columns=[column + '_x', column + '_y'], inplace=True)
    merged_df.drop(merged_df.filter(regex='_d$').columns, axis=1, inplace=True)
    merged_df['Routine_Order'] = merged_df['ROUTINE IMPLEMENTATION'].apply(lambda x: 0 if 'LI-PaRAD' in x else 1)
    merged_df.sort_values(by=['BUF SIZE B', 'Routine_Order'], inplace=True)
    merged_df.drop(columns='Routine_Order', inplace=True)

    # Write the merged dataframe to the output file
    merged_df.to_csv(output_file, sep="|", index=False)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Merge two result files based on 'BUF SIZE B' and 'ROUTINE IMPLEMENTATION'")
    parser.add_argument("input_file1", help="Path to the first input file")
    parser.add_argument("input_file2", help="Path to the second input file")
    parser.add_argument("output_file", help="Path to the output merged file")
    args = parser.parse_args()

    
    with open(args.input_file1, "r+") as f:
        d = f.readlines()
        f.seek(0)
        for l in d:
            if not l.startswith("--"):
                f.write(l)
        f.truncate()
    with open(args.input_file2, "r+") as f:
        d = f.readlines()
        f.seek(0)
        for l in d:
            if not l.startswith("--"):
                f.write(l)
        f.truncate()

    merge_files(args.input_file1, args.input_file2, args.output_file)