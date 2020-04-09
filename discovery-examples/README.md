# README #

This README would normally document whatever steps are necessary to get your application up and running.

### What is this repository for? ###

* This repository collects different examples that are used in the Discovery project.
* Version
* [Learn Markdown](https://bitbucket.org/tutorials/markdowndemo)

### How do I get set up? ###

* Summary of set up
* Configuration
* Dependencies
* Database configuration
* How to run tests
* Deployment instructions

### Contribution guidelines ###

* File Naming Conventions
    * Please create a different directory for each example. 
    * Place the file that shows the example in its original implementation including ad-hoc text comments on potential annotations (e.g. OpenMP, MPI, sequential, ...) in original.(c,cc)
    * Place the version with the intended final refactored code (eg including calls to a skeleton library) in final.c
* Directory Naming Convention
    * __SEQ__ - Original code is sequential
    * __MPI__ - Original code is MPI
    * __PTH__ - Original code is Pthread
    * __OMP__ - Original code is OpenMP
* Code review
* Other guidelines

### Who do I talk to? ###

* Repo owner or admin
* Other community or team contact

### Notes 20th November ###

_list_  - map, abstract containers with properties and operations

_histogram_ - this is map reduce, NB use of 2D array embedded as 1D, 

_jacobi_ - need for iterator patterns, double buff as a optimization, is this one or two stencils

_associative_ - reduction, properties needed, C++ traits? 

_quicksort_ - D&C pattern, also think about iterative flattened non recursive version, and find the same abstract
pattern from both

_pipelines_ - get a proper API, generic, getting rid of the threaded stuff.

