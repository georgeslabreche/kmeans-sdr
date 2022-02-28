The raw binary files in this tar.gz are always 419kB fixed in size - this is the same datachunk that we acquire on-board OPS-SAT.
There are 1492 raw files.
In the CSV, the filenames are listed and the number after the comma indicates if there is a beacon in this dataset (0=No beacon, 1=Beacon)\
Not every RAW file is present in the .CSV, some files were inconclusive and are not considered a good training input (ambiguous situations where the recording
contains only a chunk of a beacon at the beginning, to be discussed if this is valid training input or not).
