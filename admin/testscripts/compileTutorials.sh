#! /bin/bash

cd ../..

cd examples/compiler_projects

for FILE in `ls conf_*` ; do
  if [ -f $FILE ] ; then 
      echo $FILE
      ./update_makefile.sh $FILE
  fi
done

cd ../../examples/compiler_projects/kdevelop_make

for FILE in `ls` ; do
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
