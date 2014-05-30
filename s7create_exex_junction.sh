#!/bin/bash

# step 7 of http://bioinf.uni-greifswald.de/bioinf/wiki/pmwiki.php?n=IncorporatingRNAseq.Tophat

cat split/aug1.out.* | grep -P "\tintron\t" > aug1.introns.gff
cat intron_hints.gff aug1.introns.gff | perl -ne '@array = split(/\t/, $_);print "$array[0]:$array[3]-$array[4]\n";'| sort -u > introns.lst
intron2exex.pl --introns=introns.lst --seq=Jin03.masked.fa --exex=exex.fa --map=map.psl --flank=101
bowtie-build exex.fa Jin03_exex1

