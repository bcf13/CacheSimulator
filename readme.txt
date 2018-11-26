This is the MATLAB package for our paper:
“Segmentation-based registration and motion correction of 3D retinal wide
  field-of-view optical coherence tomography"
; Biomedical Optics Express Vol. 7, Issue 12, pp. 4827-4846 (2016)”
Version 0.21, April 2016

Copyright (C) 2016, Duke University; 



Please cite the above paper if you use any component of this software.

Developed by Jose Lezama <jlezama@fing.edu.uy>
 who developed the code under the supervision of Prof. Sina Farsiu and Prof. Guillermo Sapiro at Duke University.

For academic use only

* 



 The main steps of the process are
 1. segment the retinal surface
 2. Lateral registration
  2.1 saccades detection
  2.2 saccades correction
  2.3 nonrigid registration (orientation-aware Optical Flow)
  2.4 volume interpolation
 3. Axial registration
  3.1 solve linear system
  3.2 volume interpolation
 4. (optional) Write result images

* 

The method parameters are set in the file ‘lib/load_registration_parameters.m’

* 


Requires download and compilation of the SIFTFlow code from https://people.csail.mit.edu/celiu/SIFTflow/ : 
 


 1) extract dowloaded zip into aux/SIFTflow/
  2) > cd aux/SIFTflow/mexDiscreteFlow/
  3) > mex mexDiscreteFlow.cpp BPFlow.cpp Stochastic.cpp

*



 The code relies heavily on MATLAB parallelization. The recommended hardware setup is 4 to 8 CPU cores with 32 GB of RAM.

* 


Included files and folders:
  - run_XFastYFast.m:  main function for XFast-YFast registration
  - example_call.m: example on how to call run_XFastYFast
  - readme.txt: this file
  - lib/: folder with principal functions required to compute the registration
  - aux/: folder with misc. functions





THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.




