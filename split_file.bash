#!/usr/bin/env bash

message="$(git log --pretty=format:'%s' -n1)"

if [ `git status --porcelain --untracked-files=no | wc -l` = 0 ]
then
   git reset --soft HEAD^
fi

git status --porcelain --untracked-files=no | while read status file
do
   echo $status $file

   if [ "$status" = "M" ]
   then
      git add $file
      git commit -n $file -m "$file: $message"
   elif [ "$status" = "A" ]
   then
      git add $file
      git commit -n $file -m "added $file: $message"
   elif [ "$status" = "D" ]
   then
      git rm $file
      git commit -n $file -m "removed $file: $message"
   else
      echo "unknown status $file"
   fi
done
