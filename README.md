# trax
The trax library implements the logic of track guided vehicles together
with a signaling and sensor system as well as cargo transportation. It makes use
of the Nvidia PhysX engine to simulate the dynamics of trains moving along tracks 
and beyond. 

The library was used to build the railroad simulators 
<a href="https://eepshopping.de/eep_train_simulator_mission_v1_122|7683.html">EEP Train Simulator Mission</a> 
and <a href="https://eepshopping.de/eep_tsm_gotthardbahn_nordrampe_modul_erstfeld|8235.html"> EEP TSM Gotthardbahn Nordrampe Modul Erstfeld</a>. 
It also is part of the <a href="https://eepshopping.de/eep_eisenbahn_exe_professional_18_download_version|11927.html">Eisenbahn.exe Professional (EEP)</a> product line.
There is a <a href="../EisenbahnNow.pdf">whitepaper</a> that describes the basic ideas behind the library. 
The mathematical foundations for the railroad dynamics are described <a href="../TrackConstraint.pdf">here</a>.</p>

# 1. Tools

Install these tools prior to following the installation instructions for 
the project.

<ul>
	<li><b><a href="https://git-scm.com/">Git</a></b>: A version control system. Needed to get the source code from the repository.</li>
	<li>Register a <b><a href="https://github.com/">GitHub Account</a></b>: Here the 'Trend Trax Library' resides.</li>
	<li><b><a href="http://visualstudio.com/">Visual Studio 2022 Community Edition</a></b>: Download and install the community edition. We will
		only need the 'workload' called Desktop Development with C++.
	</li>
	<li><b><a href="https://cmake.org/">CMake</a></b>, a project configuration tool, will also be used by the PhysX SDK.</li>
	<li><b><a href="http://www.stack.nl/~dimitri/doxygen/">Doxygen</a></b>: A documentation generator.</li>
	<li><b><a href="https://www.python.org/">Python</a></b> will be used by the PhysX SDK.</li>
</ul>

# 2. Dependencies

Currently there are two dependencies of the library:

## Boost
<a href="https://www.boost.org">Boost</a>, referenced in the project by the environment variable BOOST_ROOT.
From that we currently need the following libraries:
<ul>
	<li>boost/math</li>
	<li>boost/property_tree</li>
	<li>boost/test</li>
</ul>
From these only the last one needs to get built. Follow the instructions for bulding and installing boost; alternatively
you can copy the

    ./Tools/BuildBoost.bat

file from the trax project (see 3. Installation) into the boost root and execute it, to simplify and speed up the process for our
purposes.

## PhysX
<a href="https://github.com/NVIDIA-Omniverse/PhysX">PhysX</a>, referenced by the environment variable PHYSX_ROOT.
Open a command prompt with admin rights and clone the main branch of the repository:

    git clone https://github.com/NVIDIA-Omniverse/PhysX.git PhysX
	cd ./PhysX/physx
	setx /M PHYSX_ROOT "%CD%"

It provides a batch file that lets you create the project files for Visual Studio; we need a '64 bit CPU only' solution.
Also the PhysX library has to link with the 'Multi-threaded DLL' runtime libary. That can be done, by editing the respective xml 
data files in the folder: 

    ./physx/buildtools/presets/public
	
PX_GENERATE_STATIC_LIBRARIES should be set to False: 
 
    <cmakeSwitch name="NV_USE_STATIC_WINCRT" value="False" comment="Use the statically linked windows CRT" />
    
Open the generated solution and build all the targets ('Release', 'Debug', 'Checked', 'Profile') for the 'INSTALL' project.

# 3. Installation

The first step would be to install the tools from the list (see above);
download, build and install the dependencies and then get the latest version of
trax from GitHub: open a new command prompt and with a valid account use git:</p>

    git clone https://github.com/Trend-Verlag/trax.git Trax2

There is a batch file (Install.bat) in the root folder of trax, that does 
necessary installations like setting environment variables, generating project 
files with CMake, building the Debug version as a compile test and then generates 
the documentation:

    cd ./Trax2
    ./Install.bat

Open the solution in the ./_Build folder, select 'Release' as a configuration,
select the 'ALL_TESTS' project and build and run it.