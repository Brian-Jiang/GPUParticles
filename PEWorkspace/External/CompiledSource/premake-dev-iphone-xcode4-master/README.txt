PREMAKE
A build configuration tool

 Copyright (C) 2002-2012 by Jason Perkins
 Distributed under the terms of the BSD License, see LICENSE.txt

 The Lua language and runtime library is (C) TeCGraf, PUC-Rio.
 See their website at http://www.lua.org/


 See the file BUILD.txt for instructions on building Premake.


 For questions, comments, or more information, visit the project
 website at http://industriousone.com/premake


This is the repo cloned from 
https://bitbucket.org/erwincoumans/premake-dev-iphone-xcode4

with the iPhone patch:
https://sourceforge.net/tracker/?func=detail&aid=3014887&group_id=71616&atid=531880
with the following description:

---------

I am submitting a patch to allow basic support to build for the iPhone platform. It currently only generate valid solution for libraries (there is no such thing as a command line tool on the iPhone :-) )

Also, I added a really function called xcodebuildsettings. This let you specify the xcode build settings like you normally do in a .xcconfig file. This alone brings a lot to the xcode generator. You can use it like the "defines" or "files" keyword.

An example on how to use it:
xcodebuildsettings
{
"ARCHS = (i386, x86_64)",
"VALID_ARCHS = (i386, x86_64)",
"SDKROOT = macosx10.5",
"ONLY_ACTIVE_ARCH = NO",
}

With this I am able to change the SDKROOT and also I am able to generate a Universal Binary (intel only 32bit-64bit excluding ppc). This is only the tip of the iceberg, you can do many other things like exclude file in certain configs only:
'EXCLUDED_SOURCE_FILE_NAMES = ("*Proxy*", "Command*")' 

I also change the way xcode id were generated to use only math.random calls and no os.time() call. Same functionality can be achieved by calling math.randomseed(1234) at the beginning of your premake4.lua script. This way if you generate twice in a row the same projects and it produce the same results.

-------


You can download the precompiled Mac OSX premake4 binary in the download section:
https://github.com/erwincoumans/premake-dev-iphone-xcode4/downloads




