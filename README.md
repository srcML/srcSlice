# srcSlice

## Description

srcSlice is a fast, lightweight srcML tool for static slicing. srcSlice reads srcML
output files and produces variable slices in JSON, these slices contain details
such as where a variable is used and defined, passed into a function as an argument,
function of origin, class it is contained in, variables that are data-dependent
of it, and potential aliases.

[Building srcSlice](#hammer_and_wrench-building-srcslice)<br>
[Using srcSlice](#computer-using-srcslice)

## :hammer_and_wrench: Building srcSlice

```bash
# Install srcML Develop if not installed on your local machine
git clone --recursive -b develop https://github.com/srcML/srcML.git
```
```bash
# Clone srcSlice repository
git clone --recursive https://github.com/srcML/srcSlice.git

# (This guide assumes the new directory is at the same directory tree level as the cloned directory)
mkdir sliceBuild
cd sliceBuild

# Run cmake to create the build files and run make to build the executable
cmake ../srcSlice
make
```

## :computer: Using srcSlice
Input: A srcML file of source code with `--position` and `--hash` options.
srcML file can be a single unit (one source code file) or an archive (multiple source code files).<br>
`srcml shell_sort.cpp -o shell_sort.cpp.xml --position --hash`

Output: A JSON file containing the slice information about each variable contained in the input file.<br>
`./srcslice -i shell_sort.cpp.xml -o results.json`
`./srcslice --help`


## Options

Display the srcslice help page<br>
`./srcslice --help`

Output debugging info to stdout<br>
`./srcslice -i shell_sort.cpp.xml --verbose`

Set the input srcML output file to read in<br>
`./srcslice -i shell_sort.cpp.xml`

Write the output to file. By default, it writes to standard output<br>
`./srcslice -i shell_sort.cpp.xml -o results.json`

## Examples

Generate variable slices and write output to standard output<br>
`./srcslice -i shell_sort.cpp.xml`

Generate variable slices and write output to **slices.json**<br>
`./srcslice -i shell_sort.cpp.xml -o slices.json`


## :scroll: srcSlice Output

<table>
  <thead>
    <tr>
      <th>Attribute</th>
      <th>Description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>File</code></td>
      <td>File path of the source code the slice variable originates from</td>
    </tr>
    <tr>
      <td><code>Language</code></td>
      <td>Language of the source code file</td>
    </tr>
    <tr>
      <td><code>Namespace</code></td>
      <td>List of namespaces the slice variable is contained in</td>
    </tr>
    <tr>
      <td><code>Class</code></td>
      <td>Name of the class containing the slice variable</td>
    </tr>
    <tr>
      <td><code>Function</code></td>
      <td>Name of the function containing the slice variable</td>
    </tr>
    <tr>
      <td><code>Type</code></td>
      <td>Data type of the slice variable</td>
    </tr>
    <tr>
      <td><code>Name</code></td>
      <td>Variable name of the slice variable</td>
    </tr>
    <tr>
      <td><code>Dependence</code></td>
      <td>
        A list of variable-line pairs, each containing the name of the variable that 
        is data-dependent on the slice variable and the line where it occurs
      </td>
    </tr>
    <tr>
      <td><code>Aliases</code></td>
      <td>
        A list of potential targets of a pointer or reference (may differ based on 
        the Points-To Analysis Algorithm chosen: Andersen's, Steengaard's, etc.)
      </td>
    </tr>
    <tr>
      <td><code>Calls</code></td>
      <td>
        A list of function calls where the slice variable is used as an argument 
        (includes function name, parameter index, and line of function definition)
      </td>
    </tr>
    <tr>
      <td><code>Use</code></td>
      <td>A set of line numbers where the slice variable is used</td>
    </tr>
    <tr>
      <td><code>Definition</code></td>
      <td>A set of line numbers where the slice variable is defined or redefined</td>
    </tr>
  </tbody>
</table>

## Slice Output Example
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
    "type":"int*",
    "name":"array",
    "dependence":[{"temp":21}],
    "aliases":[],
    "calls":[],
    "use":[18,21,22,32],
    "definition":[5,11,22,23,35]
}
```

</td>
</tr>
</table>