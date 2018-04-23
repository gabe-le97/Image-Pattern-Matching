# Image Pattern Matching
Finds patterns in an "image" (a grid of ascii characters) and 
reports the number of matches each pattern has on the images and where
they occur. The result is printed in the console and saved in output.txt files.
This program is merely an abstraction and simplification of the real life problem.
***
This program takes as parameters: __./filename pathToImageFiles pathToPatternFiles__
***
__Bash Script__: changes the windows image.txt & pattern.txt file 
newlines to the proper linux endlines
***
__Version 1__ - Child processes are clones of their parents:
* Create child processes equal to the number of image files <br>
* Each child will check to see if the pattern files match with their image <br>
* Results are written to files named after the PID of each process <br>

__Version 2__ - Child Processes load a separate program
* main.c -> creates the child processes which launch the helper program for the 
processes to execute. Then wait for the child processes to finish
* helper.c -> execute the pattern search and write the matches to the output files

__Version 3__ - Child processes report their results via a pipe
* Child processes send their report via an unnamed pipe
