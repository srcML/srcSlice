# srcSlice
## Lightweight Tool for Slicing
NOTE: Current version of srcSlice in the master branch is a new version that will<br>
take advantage of the event dispatcher framework. It is under construction, but it should work.

If you'd like the old version of srcSlice, switch to the "old" srcSlice branch.<br>
This is the version we presented at [ICSE 16](https://www.cs.kent.edu/~jmaletic/papers/ICSE16.pdf).

*NOTICE: srcSlice is a srcML tool, ensure you have srcML installed as stated below*

## :hammer_and_wrench: Installation & Build
1. Install [srcML develop](https://github.com/srcML/srcML/tree/develop) if it is not installed on your local machine

2. Clone the repository with `git clone --recursive` into the desired directory.<br>
   Make sure you include the `--recursive` as srcSlice includes submodule(s).

3. Outside of the cloned directory, create a new directory for the build.<br>
   (This guide assumes the new directory is at the same directory tree level as the cloned directory)

4. Enter the new directory (not the cloned one) and type `cmake ../{cloned directory}`

5. After cmake runs, simply type `make` and all files should be built.
    * For a list of options for make run `make help`

6. Once everything is built, you can find your executable at `./bin/srcslice`.

## :computer: To run srcSlice
1. Create an output srcML file, `srcml [input file] -o [outfile name] --position`<br>
    * You MUST include `--position` as srcSlice uses the extra data produced by this flag

2. Locate your `srcslice` binary and execute
    * For a list of options run `srcslice [-h/--help]`