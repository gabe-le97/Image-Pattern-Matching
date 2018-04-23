#-----------------------------------------------------
# Created by Gabe Le
# 3/31/18
#......................................................
# A script for modifying files with window line endings
# 	and replacing the line endings with Unix line endings
#
# My test arguments
# ./script.sh /Users/gabe/Desktop/Test/Data/Images/ /Users/gabe/Desktop/Test/Data/Patterns
#-----------------------------------------------------

# check if the correct number of  arguments is given 
if [ "$#" -ne 2 ] || ! [ -d "$2" ]; then
  echo "Enter the correct arguments" >&2
  exit 1
fi


# Function that removes line endings from windows .txt files
modify_windows() {
	for filename in /Users/copy/*.txt; do
		tr -d '\015' <${COPYPATH}/$(basename "$filename" .txt).txt >> ${PLACEHOLDER}/$(basename "$filename" .txt).txt
	done
}

# Getting the directoy of the image and path files (image)
IMAGEPATH=$1
PATTERNPATH=$2

# Make a new directory to make a copy of the files and modify them
COPYPATH=/Users/copy
PLACEHOLDER=${COPYPATH}/temp
sudo install -d -m 0777 $COPYPATH
sudo install -d -m 0777 $PLACEHOLDER
sudo cp -r $IMAGEPATH/. $COPYPATH/

# Replace windows whiteline to unix for the image files
modify_windows $COPYPATH $PLACEHOLDER

# Move all modified files back to the original files 
mv ${PLACEHOLDER}/* ${IMAGEPATH}

# Remove all .txt files from the copied directory
sudo rm -r ${COPYPATH}/*.txt

# Do the same for the Pattern .txt files
sudo cp -r $PATTERNPATH/. $COPYPATH/

modify_windows $COPYPATH $PLACEHOLDER

mv ${PLACEHOLDER}/* ${PATTERNPATH}

sudo rm -r $COPYPATH

echo 'Done modifying files'