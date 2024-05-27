# srcSlice
Lightweight tool for slicing

NOTE: Current version of srcSlice in the master branch is a new version that will take advantage of the event dispatcher framework. It is under construction, but it should work. Give it a try!

If you'd like the old version of srcSlice, switch to the "old" srcslice branch. This is the version we presented at ICSE 16.

*NOTICE: srcslice is a srcml tool, ensure you have srcml installed as stated below!*

To build srcSlice:

1. Clone the repository with 'git clone --recursive' into the desired directory. Make sure you include the --recursive as srcSlice includes a submodule that must also be cloned.

2. Outside of the cloned directory, create a new directory for the build.  (This guide assumes the new directory is at the same directory tree level as the cloned directory)

3. Enter the new directory (not the cloned one) and type 'cmake ../{cloned directory}'

4. After cmake runs, simply type 'make' and all files should be built.  

5. Once everything is built, go into the 'bin' folder and that's where the executable will be.

To run srcSlice:

1. Install [srcml](https://www.srcml.org/#download) if it is not installed on your local machine

2. Create an output srcml file, `srcml [input file] -o [outfile name] --position`<br>
    * You MUST include `--position` when running the srcml binary like above, otherwise<br>
      the srcSlice output will have missing data!

3. Locate your `srcslice` binary and execute! `srcslice [srcml input file]`
    * This branch of srcslice is equipped with a help menu to show various options!