#! /bin/bash

cd ../..

cd examples/compiler_projects

for FILE in `ls conf_*_*` ; do
  if [ -f $FILE ] ; then 
      echo $FILE
      ../../library/xgpl_src/build/projectGeneration/update_makefile.sh $FILE
  fi
done

echo "conf_PcCompilation"
../../library/xgpl_src/build/projectGeneration/update_makefile.sh conf_PcCompilation

cd ../../examples/compiler_projects/kdevelop_make

for FILE in `ls -d *_*` ; do
  if [ -d $FILE ] ; then 
      echo $FILE
      cd $FILE
      rm objects*/*
      make 
      if [ $? -ne 0 ]; then
          echo ERROR in $FILE
          exit
      fi
      make clean
      cd ..
  fi
done

echo PcCompilation
cd PcCompilation
rm objects*/*
make 
if [ $? -ne 0 ]; then
	echo ERROR in PcCompilation
	exit
fi
make clean
cd ..
