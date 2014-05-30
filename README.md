Jin-annot
=========

Scripts for annotating a genome with RNA-seq and Protein sequence data.

This repository holds scripts and programs that was used to 
annotate a plant genome using RNA-seq and annotation of other 
sequenced genomes.

The scripts are making it easy to execute what are desribed at various pages.
Important pages are:
http://bioinf.uni-greifswald.de/bioinf/wiki/pmwiki.php?n=IncorporatingRNAseq.Tophat
http://bioinf.uni-greifswald.de/bioinf/wiki/pmwiki.php?n=Augustus.Augustus
http://www.repeatmasker.org/

Most of the software used in these scripts are installed with LPM.
http://www.kasahara.ws/lpm/

# Input files
Jin03.fa (the genome not included)
RNA-seq-list (the list of RNA-seq data; index number and description)
The raw RNA-seq data are organized in subdirectories as by index numbers
```
Sample_idx1/
  idx1_ATCACG_L006_R1_001.fastq.gz
  idx1_ATCACG_L006_R2_001.fastq.gz
```

# RepeatMask
Masking repeats is recommended by Augustus author. 
To mask repeat properly, we need a proper species specific repeat profile.

## Repeat Modeling
* Jin3RM.job
* Jin3RM2.job

## Actual Repeat Masking
* repeatmask2.job
note the directory name RM_65194.FriMay90932462014 should be changed to
what was made by repeat modeler.

# CEGMA
* cegma03.job

# Interleve read1 and read2 of RNA-seq while trimming the adapter sequence
* utadapt-pe.c

# rename the tail of readname from /1,/2 to -1,-2
* rename-aug.job

# Iterative Augustus
## Map the RNA-seq data to the masked genome with tophat
* tophat2masked.job

## extract exon junctions
## map the RNA-seq to exon junctions using bowtie2

## Map protein sequence data to genome using exonerate
## construct combined hint data.
## split genome
## Run Augustus on split genome
