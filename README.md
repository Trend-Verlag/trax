# trax

The trax library implements the logic of track guided vehicles together
with a signaling and sensor system as well as cargo transportation. It makes use
of the NVIDIA PhysX engine to simulate the dynamics of trains moving along tracks 
and beyond. 

The library was used to build the railroad simulators 
<a href="https://eepshopping.de/eep_train_simulator_mission_v1_122|7683.html">EEP Train Simulator Mission</a> 
and <a href="https://eepshopping.de/eep_tsm_gotthardbahn_nordrampe_modul_erstfeld|8235.html"> EEP TSM Gotthardbahn Nordrampe Modul Erstfeld</a>. 
It also is part of the <a href="https://eepshopping.de/eep_eisenbahn_exe_professional_18_download_version|11927.html">Eisenbahn.exe Professional (EEP)</a> product line.
There is a <a href="https://www.trendverlag.com/Trax/Doc/BlogArticle/traxLibrary.html">whitepaper</a> that describes the basic ideas behind the library. 
The mathematical foundations for the railroad dynamics are described <a href="https://www.trendverlag.com/Trax/Doc/TrackConstraint.pdf">here</a>.</p>

The [Trax GitHub Project](https://github.com/Trend-Verlag) produces a documentation from the 
source code using Doxygen. It contains a medium level overview as well as detailed description 
of the API. A ready generated version is available online: 
[Trax Library Documentation](https://www.trendverlag.com/Trax/Doc/API/html/index.html).

## 1. Tools

Install these tools prior to following the installation instructions for 
the project.

**Required:**
- <a href="https://git-scm.com/">Git</a>: A version control system. Needed to get the source code from the repository.
- Register a <a href="https://github.com/">GitHub Account</a>: Here the 'Trend Trax Library' resides.
- <a href="https://cmake.org/">CMake</a>: A project configuration tool, will also be used by the PhysX SDK.
- <a href="http://visualstudio.com/">Visual Studio 2022 Community Edition</a>: Download and install the community edition. We will
		only need the 'workload' called Desktop Development with C++.
- <a href="https://www.python.org/">Python</a> will be used by the PhysX SDK.
		
**Optional:**
- <a href="https://developer.nvidia.com/physx-visual-debugger">PhysX Visual Debugger</a>: A tool that renders a PhysX simulated scene. Can be used with the trax tests.
- <a href="http://www.stack.nl/~dimitri/doxygen/">Doxygen</a>: A documentation generator.
- <a href="https://graphviz.org/">Graphviz</a> - for class hierarchy and include graphs in generated documentation
  - Without Graphviz: Basic text diagrams will be generated instead
- <a href="https://www.java.com">Java Runtime (JRE 8+)</a> - required for PlantUML diagrams in documentation
  - PlantUML JAR is included in `Tools/plantuml-mit-1.2025.10.jar`
  - No separate PlantUML installation needed

## 2. Dependencies

Currently there are two dependencies of the library:

### Boost
<a href="https://www.boost.org">Boost</a>, referenced in the project by the 
environment variable BOOST_ROOT. From that we currently need the following libraries:
<ul>
	<li>boost/math</li>
	<li>boost/program_options</li>
	<li>boost/property_tree</li>
	<li>boost/test</li>
</ul>
From these only the last one needs to get built. Follow the instructions for bulding 
and installing boost; alternatively, to simplify and speed up the process for our
purposes, you can copy the

    ./Tools/BuildBoost.bat

file from the trax project (see 3. Installation) into the boost root and execute it.

### PhysX
<a href="https://github.com/NVIDIA-Omniverse/PhysX">PhysX</a>, referenced by the 
environment variable PHYSX_ROOT. Open a command prompt <b>with administrator rights</b> 
and clone the main branch of the repository:

    git clone https://github.com/NVIDIA-Omniverse/PhysX.git PhysX
	cd ./PhysX/physx
	setx PHYSX_ROOT "%CD%"

It provides a batch file that lets you create the project files for Visual Studio; we 
need a '64 bit CPU only' solution. Also the PhysX library has to link with the 
'Multi-threaded DLL' runtime libary. That can be done, by editing the respective xml 
data files in the folder: 

    ./physx/buildtools/presets/public
	
NV_USE_STATIC_WINCRT should be set to 'False': 
 
    <cmakeSwitch name="NV_USE_STATIC_WINCRT" value="False" comment="Use the statically linked windows CRT" />
    
Open the generated solution and build all the targets ('Release', 'Debug', 'Checked', 
'Profile') for the 'INSTALL' project.

## 3. Installation

The first step would be to install the tools from the list (see above);
download, build and install the dependencies and then get the latest version of
trax from GitHub: open a new command prompt and with a valid account use git:

    git clone https://github.com/Trend-Verlag/trax.git Trax3

There is a batch file (Install.bat) in the root folder of trax, that does 
necessary installations, like setting environment variables, generating project 
files with CMake, building the Debug version as a compile test and generates 
the documentation:

    cd ./Trax3
    ./Install.bat

Open the solution in the ./_Build folder, select 'Release' as a configuration,
select the 'ALL_TESTS' project and build and run it.

## 4. External Resources

### Documentation

- The [Trax GitHub Project](https://github.com/Trend-Verlag) produces a documentation from the 
source code using Doxygen. It contains a medium level overview as well as detailed description 
of the API.
- A ready generated (maybe slightly outdated) version of the documentation is available online: 
[Trax Library Documentation](https://www.trendverlag.com/Trax/Doc/API/html/index.html).

### Whitepapers 
- The [Trax Library Whitepaper](https://www.trendverlag.com/Trax/Doc/BlogArticle/traxLibrary.html) 
provides an overview of the basic concepts and architecture.
- The [Trax Library Book](https://www.trendverlag.com/Trax/Doc/Book/start.html) offers a more 
in-depth exploration.
- Detailed mathematical descriptions of the track constraint solver can be found in the 
[Track Constraint PDF](https://www.trendverlag.com/Trax/Doc/TrackConstraint.pdf).

### Blogs
- [Simulating Railroads with OpenUSD](https://developer.nvidia.com/blog/simulating-railroads-with-openusd/): 
An article discussing the integration of Trax with OpenUSD for railroad simulation.
- [YouTube Playlist](https://youtube.com/playlist?list=PLvSXZtqqeOvBoA_W5E5gI6Hz2sxWsNI45&si=oXRumqXrSvRmT5SS)

### Products Using Trax
- [Eisenbahn.exe Professional (EEP)](https://eepshopping.de/eep_eisenbahn_exe_professional_18_download_version|11927.html)
- [EEP Train Simulator Mission](https://eepshopping.de/eep_train_simulator_mission_v1_122|7683.html)
- [EEP TSM Gotthardbahn Nordrampe](https://eepshopping.de/eep_tsm_gotthardbahn_nordrampe_modul_erstfeld|8235.html)
- [Building a Digital Twin of the German Rail Network](https://www.nvidia.com/en-us/on-demand/session/gtcspring23-s51560/)
