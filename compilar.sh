/opt/pccts/bin/antlr -gt $1.g
/opt/pccts/bin/dlg -ci parser.dlg scan.c
g++ -Wno-write-strings -o $1 $1.c scan.c err.c -I /home/soft/PCCTS_v1.33/include/