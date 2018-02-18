git config --global credential.helper cache

if [ "$1" = "commit" ]
then
  git add .

  if [ -z "$2" ]
  then
    git commit -m "add"
  else
    git commit -m "$2"
  fi
  git push origin master

elif [ "$1" = "pull" ]
then
    git pull origin master
else
    echo "Not Cool"
fi
