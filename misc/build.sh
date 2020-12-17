cwd=$(pwd)

echo $cwd
cd generated/
echo Entering directory \'$(pwd)\'
make config=$1
echo Leaving  directory \'$(pwd)\'
cd $cwd
