# srcSlice

## Description

srcSlice is a fast, lightweight srcML tool for static slicing. srcSlice reads srcML
output files and produces variable slices in JSON, these slices contain details
such as where a variable is used and defined, passed into a function as an argument,
function of origin, class it is contained in, variables that are data-dependent
of it, and potential aliases.

## Table of Contents:
- [Building srcSlice](#building-srcslice)
- [Using srcSlice](#using-srcslice)
- [Options](#options)
- [Examples](#examples)
- [Output](#srcslice-output)

<br>

## Building srcSlice

### Prepare dependencies:
```bash
# Install srcML Develop if not installed on your local machine, after running 'make' when building srcML run 'make install' to ensure cmake can find the srcML package when building srcSlice
git clone --recursive -b develop https://github.com/srcML/srcML.git
```

### Preparing srcSlice:
```bash
# Clone srcSlice repository
git clone --recursive https://github.com/srcML/srcSlice.git

# (This guide assumes the cloned directory and build directory are at the same working directory)
mkdir build
cd build

# Run cmake to create the build files and run make to build the executable
cmake ../srcSlice

# You can run make but it will take longer
make srcslice
```

<br>

## Using srcSlice
Input: A srcML file of source code with `--position` and `--hash` options.
srcML file can be a single unit (one source code file) or an archive (multiple source code files).
```text
srcml shell_sort.cpp -o shell_sort.cpp.xml --position --hash
```

Output: A JSON file containing the slice information about each variable contained in the input file.<br>
```text
./srcslice -i shell_sort.cpp.xml -o results.json
```

<br>

## Options

Display the srcslice help page
```
./srcslice --help
```

Output debugging info to stdout<br>
```
./srcslice -i shell_sort.cpp.xml --verbose
```

Set the input srcML output file to read in<br>
```
./srcslice -i shell_sort.cpp.xml
```

Write the output to file. By default, it writes to standard output<br>
```
./srcslice -i shell_sort.cpp.xml -o results.json
```

<br>

## Examples

Generate slices and write output to standard output<br>
```
./srcslice -i shell_sort.cpp.xml
```

Generate slices and write output to **slices.json**<br>
```
./srcslice -i shell_sort.cpp.xml -o slices.json
```

<br>

## srcSlice Output

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
        A list of variable-position pairs, containing:
        <ul>
        <li>Name of Variable Data-Dependent of the Slice</li>
        <li>Position of the Dependence</li>
        </ul>
      </td>
    </tr>
    <tr>
      <td><code>Aliases</code></td>
      <td>
        A list of potential targets of a pointer or reference
      </td>
    </tr>
    <tr>
      <td><code>Calls</code></td>
      <td>
        A list of function calls where the slice variable is used as an argument.<br>
        This includes:
        <ul>
        <li>Function Name</li>
        <li>Argument Index</li>
        <li>Position of Function Definition</li>
        <li>Position of Invokation</li>
        </ul>
      </td>
    </tr>
    <tr>
      <td><code>Use</code></td>
      <td>A set of positions where the slice variable is used</td>
    </tr>
    <tr>
      <td><code>Definition</code></td>
      <td>A set of positions where the slice variable is defined or redefined</td>
    </tr>
  </tbody>
</table>