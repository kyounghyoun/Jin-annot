Jin-annot
=========

Scripts for annotating a genome with RNA-seq and Protein sequence data.

This repository holds scripts and programs that was used to 
annotate a plant genome using RNA-seq and annotation of other 
sequenced genomes.

The scripts are making it easy to execute what are desribed at various pages.
Important pages are:
- http://bioinf.uni-greifswald.de/bioinf/wiki/pmwiki.php?n=IncorporatingRNAseq.Tophat
- http://bioinf.uni-greifswald.de/bioinf/wiki/pmwiki.php?n=Augustus.Augustus
- http://www.repeatmasker.org/

Most step are encupsulated in job scripts that can be run in a SGE/UGE environment,
while some are left for command line execution.
The job scripts were run on the phase 1 system in NIG super computer. Some parameters,
especially job parameters regarding parallel environment need adjustment to the
system you use.

Most of the software used in these scripts are installed with LPM.
- http://www.kasahara.ws/lpm/

## Input files
* Jin03.fa (the genome not included)
* RNA-seq-list (the list of RNA-seq data; index number and description)
The raw RNA-seq data are organized in subdirectories as by index numbers
```
Sample_idx1/
  idx1_ATCACG_L006_R1_001.fastq.gz
  idx1_ATCACG_L006_R2_001.fastq.gz
```

## RepeatMask
Masking repeats is recommended by Augustus author. 
To mask repeat properly, we need a proper species specific repeat profile.

### Repeat Modeling
* Jin3RM.job
* [Jin3RM2.job](Jin3RM2.job)

### Actual Repeat Masking
* [repeatmask2.job](repeatmask2.job)

Note the directory name RM_65194.FriMay90932462014 should be changed to
what was made by repeat modeler.

## CEGMA
* [cegma03.job](cegma03.job)

## Interleave read1 and read2 of RNA-seq while trimming the adapter sequence
* [cutadapt-pe.c](cutadapt-pe.c)

## Rename the readname from /1,/2 to -1,-2
* [rename-aug.job](rename-aug.job)

## Iterative Augustus
### Initial training of Augustus with CEGMA results
    new_species.pl --species=Jinfusca3   
* [etrain.job](etrain.job)

We need to train augustus with some data. 
At begining just use the ultra conserved genes from cegma. 

The instruction was given at:
- http://bioinf.uni-greifswald.de/bioinf/wiki/pmwiki.php?n=Augustus.CEGMATraining


### Prepare Bowtie2 index
* [bb.job](bb.job)

Because tophat2 use bowtie2, bowtie2 index should prepared in advance.
This is run as a single process, while actuall mapping process can be run
in parallel, using the same index and separate query RNA-seq data.
While this produces index files starting Jin03_m_db,
tophat process requires presence of Jin03_m_db.fa

    ln Jin03.masked.fa Jin03_m_db.fa

should be performed.

### Map the RNA-seq data to the masked genome with tophat
* [tophat2masked.job](tophat2masked.job)

* [filter.job](filter.job)

Select good mapping reads. (--paired for paired end read data)
This phase is run independently for each index of RNA-seq data.

* [merge.job](merge.job)

Merge the filtered data and sort. Because we merge, we have to
wait all filter job finished and run as a separte single job.

### Generate hints file from the mapped data
* [genhint.job](genhint.job)

### Split genome
* [flatsplitbysize.rb](flatsplitbysize.rb)
```
mkdir -p split
cd split
cp -p ../Jin03_m_db.fa .
ruby ../flatsplitbysize.rb Jin03_m_db.fa 1000000
```
Split the genome fasta file in a chunk of 1 Mb.
When the scaffold is large one scaffold is in the split file,
while multiple scaffold is put in a single file until the cumulative
size reach 1 Mb.

### Perform 1st augustus run
* augustus1.job
* [split/augsp.job](split/augsp.job)

Initially, augustus1.job was used. This took a day or more.
After realizing that the genome may split and augustus is run in parallel
split/augsp.job was written.

### Extract exon junctions
* create_exex_junction.sh 
* [s7create_exex_junction.sh](s7create_exex_junction.sh)

Step 7 

### Map the RNA-seq to exon junctions using bowtie2
* bowtie2exexj.job
* [s8bowtie2exexj.job](s8bowtie2exexj.job)

Map to exon junction database and adjust the coordinate back to the global coordinate.

* [discard_intron_map.job](discard_intron_map.job)

_thm/accepted_hits.noN.bam

merge2.job

Discard the intron map data from original mapping.

* s9join.job
* s10filter.job
* s12augustus2.job
* s13joingff.job
* s14togbandretrain.job

### Map protein sequence data to genome using exonerate
* exonerateAt.job
* exoneratePp.job

Each of these jobs does map the protein sequences to the masked draft genome.

* exonerate-merge-to-hints.job

collects all the output file of the exonerateAt.job and exoneratePp.job
and converts to hint file.  The output files are identified by the name exonerate??.job.o*.

### Convert repeatmasker output to hints.

* rm2hints.job

### Construct combined hint data.

* mergehints.job

### Run Augustus on split genome
* s22baugustus3.job

* s23bjoingff.job

Combine the split prediction over the genome.
```
s22augustus3.job
s23joingff.job
```
These output in gff2 format whereas the above is configured to write gff3 format.

## unexplained
```
* exclude_intron_map.sh
s15optimize_augustus.job
```
