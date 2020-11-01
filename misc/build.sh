cwd=$(pwd)

echo $cwd
cd Generated/
echo Entering directory \'$(pwd)\'
make config=$1
echo Leaving  directory \'$(pwd)\'
cd $cwd
