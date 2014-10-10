Oculus
=========
A KL extension for the Oculus Rift SDK

Oculus allows you to read the head tracking information from the Oculus Rift, and adds functionality to the InlineDrawing realtime rendering for two eyed warped display.

extension status
================

This extension has been built and tested against Fabric Engine 1.3.0. Higher release might work, but this hasn't been validated. Feel free to use this code for reference or training purposes, it's meant to be an example of how to integrate custom libraries into KL.

structure
=========

The Oculus extensions uses the KL2EDK tool to generate the required C++ code out of a json codegen file. This makes it easy to add functionality to the extension later, or support future versions of Oculus. You won't find any .cpp files in the *src* folder, the source code for certain non-automatic function bodies can be found in the .codegen.json file in the *src* folder.

The latest KL2EDK documentation can be found here: http://documentation.fabricengine.com/FabricEngine/latest/HTML/ExtensionAuthoringGuide/kl2edk.html

supported platforms
===================

To date all three major platforms (windows, linux, osx) are supported, if you build the thirdparty dependencies for the corresponding platform.

building
========

A scons (http://www.scons.org/) build script is provided. Oculus depends on
* The Oculus Rift SDK (0.4.2 or higher)

To inform scons where to find the Fabric Engine includes as well as the thirdparty libraries, you need to set the following environment variables:

* FABRIC_DIR: Should point to Fabric Engine's installation folder.
* OCULUS_DIR: Should point to the root of the Oculus SDK.

The temporary files will be built into the *build* folder, while the structured output files will be placed in the *stage* folder.

Note: The fabric.sh exists purely for Fabric Engine internal use.

testing
=======

The extension repository also contains a series of unit tests, with which you can validate a successful build, or simply use them for reference on how to use the extension. to launch all unit tests simply use python like this:

    python test/test_all.py

license
==========

The license used for this extensions is the new BSD license. You can find it in the root folder of the repository.
