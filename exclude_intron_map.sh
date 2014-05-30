#!/bin/bash
# step 9 http://bioinf.uni-greifswald.de/bioinf/wiki/pmwiki.php?n=IncorporatingRNAseq.Tophat
# discard intron containing alignments from the original bam file
bamtools filter -in Jin03RNAs.ms.bam -out Jin03RNA.noN.bam -script $HOME/lcl/opt/augustus/current/auxprogs/auxBamFilters/operation_N_filter.txt &

# create a bam file with header from the bowtie.global.sam file
#cat header.txt bowtie.global.*.sam > bowtie.global_h.sam
#samtools view -bS -o bowtie.global.h.bam bowtie.global_h.sam
wait
