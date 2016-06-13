#!/bin/bash
# Usage: mf_extract_all.sh <JSON file> <base name> ['<args>']

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

# Extract all data.
$DIR/mf_extract.sh -likwid $1 $3 $4 $5 $6 > $2-MFRAPL.csv
$DIR/mf_extract.sh -external $1 $3 $4 $5 $6 > $2-MFEXTERNAL.csv
$DIR/mf_extract.sh -cluster $1 $3 $4 $5 $6 > $2-MFCLUSTER.csv

