import csv
import os
import pandas as pd

folderpath = 'data'

csvfiles = [f for f in os.listdir(folderpath) if f.endswith('.csv')]

dflist = []

for file in csvfiles:
    fp = os.path.join(folderpath, file)
    df = pd.read_csv(fp)
    fn = file[4:7]
    df["outgrid"] = fn
    dflist.append(df)

combdf = pd.concat(dflist, ignore_index=True)

combdf.to_csv('final.csv', index=False)