#!/bin/sh 
# run simulation

CVERDIR=~/gplcver-2.12a.src
CVER=$CVERDIR/bin/cver

if test ! -f $CVER
then
  echo "ERROR - there is no cver in $CVERDIR/bin"
  echo "Must make in src/ directory" 
  exit;
fi

$CVER +loadvpi=./trace32_verilog_transactor.so:vpi_compat_bootstrap trace32_jtag_tb.v

echo ">>>> run test tap finished"
echo " "
