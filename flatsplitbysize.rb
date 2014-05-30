#!/usr/bin/ruby
require 'bio'

ff = Bio::FlatFile.open(nil, ARGV[0])
maxsize = ARGV[1].to_i 
maxsize = 10000000 if maxsize == nil or  maxsize < 1 
filecount=0
cumsize=maxsize
while fe = ff.next_entry
  if cumsize >= maxsize then
    filecount += 1
    of=open(ARGV[0]+"."+filecount.to_s,"w")
    cumsize = 0
  end
  of.print fe.to_s
  cumsize += fe.seq.length
end
