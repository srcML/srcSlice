# srcSlice
## Lightweight Tool for Static Slicing
Input: A srcML file of source code with `--position` and `--hash` options.
srcML file can be a single unit (one source code file) or an archive (multiple source code files).

Output: A JSON file containing the slice information about each variable contained in the input file.

## :hammer_and_wrench: Installation & Build
1. Install [srcML develop](https://github.com/srcML/srcML/tree/develop) if it is not installed on your local machine

2. Clone the repository with `git clone --recursive` into the desired directory.
Make sure you include the `--recursive` as srcSlice includes submodule(s)

3. Outside of the cloned directory, create a new directory for the build.
(This guide assumes the new directory is at the same directory tree level as the cloned directory)

4. Enter the new directory (not the cloned one) and type `cmake ../{cloned directory}`

5. After cmake runs, simply type `make` and all files should be built
    * For a list of options for make run `make help`

6. Once everything is built, you can find your executable at `./bin/srcslice`

## :computer: To run srcSlice
1. Create an output srcML file, `srcml [input file] -o [outfile name] --position --hash`
    * You must include `--position` and `--hash` flags as srcSlice uses the extra data produced by these flags

2. Locate your `srcslice` binary and execute
    * `./srcslice -i [outfile name]`
    * `./srcslice -i [outfile name] -o results.json`
    * For a list of options run `srcslice [-h/--help]`

## :scroll: srcSlice Output
<style>
    .scrollable-item {
        height: 300px; /* Adjust height for vertical scrolling */
        overflow-y: auto; /* Enable vertical scrollbar */
    }
    pre {
        /* inline CSS for rendering the code blocks*/
        width: 600px; /* Adjust the width as needed */
        overflow-x: auto; /* Add horizontal scroll if content exceeds width */
    }
</style>

<table>
<tr>
<th>Slice Template</th>
<th>Description</th>
</tr>
<tr>
<td>

```json
"Slice Identifier":{
    "file":"",
    "language":"",
    "namespace":[],
    "class":"",
    "function":"",
    "type":"",
    "name":"",
    "dependence":[],
    "aliases":[],
    "calls":[],
    "use":[],
    "definition":[]
}
```

</td>
<td>
<div class="scrollable-item">

* `Slice Identifier` -- Follows the format: "variableName--initialDeclarationLine--srcmlHash"
    * srcml hash - SHA-1 hash generated based on the contents of the source-code file (srcml attribute)

* `File` -- File-Path of the source code the slice variable originates from

* `Language` -- Language of the source code file

* `Namespace` -- List of namespaces the slice variable is contained in

* `Class` -- Name of the class containing the slice variable

* `Function` -- Name of the function containing the slice variable

* `Type` -- Data-Type of the slice variable

* `Name` -- Variable name of the slice variable

* `Dependence` -- A list of variable-line pairs, each pair contains the name of the variable
that is data-dependent of the slice variable and the line number where the relation was formed

* `Aliases` -- A list of potential targets of a pointer or reference (the alias output may differ based on desired Points-To Analysis Algorithm chosen)
    - Andersen's (more output, higher complexity)
    - Steengaard's (less output, lower complexity)
    - Others can be potentially supported

* `Calls` -- A list of function calls where the slice variable is used as an argument (includes function name, parameter index, line of function definition)

* `Use` -- A set of line numbers where the slice variable is used

* `Definition` -- A set of line numbers where the slice variable is defined or redefined

</div>
</td>
</tr>
</table>

## Example Output
<table>
<tr>
    <th>Source</th>
    <th>Slice</th>
</tr>
<tr>
<td>

```c++
1       #include <iostream>
2
3       int main() {
4           int size = 10;
5           int* array = new int[size];
6           // Input
7           std::cout << "\nHow many numbers do want to enter? : ";
8           std::cin >> size;
9           std::cout << "\nEnter the numbers for unsorted array : ";
10          for (int i = 0; i < size; i++) {
11              std::cin >> array[i];
12          }
13
14          // Sorting
15          for (int i = size / 2; i > 0; i = i / 2) {
16              for (int j = i; j < size; j++) {
17                  for (int k = j - i; k >= 0; k = k - i) {
18                      if (array[k] < array[k + i]) {
19                          break;
20                      } else {
21                          int temp = array[k + i];
22                          array[k + i] = array[k];
23                          array[k] = temp;
24                      }
25                  }
26              }
27          }
28
29          // Output
30          std::cout << "\nSorted array : ";
31          for (int i = 0; i < size; ++i) {
32              std::cout << array[i] << "\t";
33          }
34
35          delete[] array;
36          return 0;
37      }
```

</td>
<td>

```json
"array-5-f7d5a6322bb7877eb114ddf4ff44bd4e79e9fd09":{
    "file":"shell_sort.cpp",
    "language":"C++",
    "namespace":[],
    "class":"",
    "function":"main",
    "type":"int *",
    "name":"array",
    "dependence":[{"temp":23}],
    "aliases":[],
    "calls":[],
    "use":[18,21,32],
    "definition":[5,11,22,23,35]
}
```

</td>
</tr>
</table>

## :closed_book: Manual

## DESCRIPTION

srcSlice is a fast, lightweight static slicing srcML tool. srcSlice reads srcml
output files and produces variable slices in JSON, these slices contain details
such as where a variable is used and defined, passed into a function as an argument,
function of origin, class it is contained in, variables that are data-dependent
of it, and potential aliases

## OPTIONS

`-h`, `--help`<br>
&nbsp;&nbsp;&nbsp;&nbsp;Display the srcslice help page

`-v`, `--verbose`<br>
&nbsp;&nbsp;&nbsp;&nbsp;Output debugging info to stdout

`-i` **inFile**, `--input`=**inFile**<br>
&nbsp;&nbsp;&nbsp;&nbsp;Set the input srcml output file to read in *(when creating the srcml input file use the --position and --hash flags)*

`-o` **outFile**, `--output`=**outFile**<br>
&nbsp;&nbsp;&nbsp;&nbsp;Write the output to file. By default, it writes to standard output

## EXAMPLES

**srcslice** -i quicksort.cpp.xml<br>
&nbsp;&nbsp;&nbsp;&nbsp;Generate variable slices and write output to standard output

**srcslice** -i quicksort.cpp.xml -o slices.json<br>
&nbsp;&nbsp;&nbsp;&nbsp;Generate variable slices and write output to **slices.json**