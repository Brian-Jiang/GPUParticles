
import os
import sys

sys.path.append(os.path.normpath(os.path.join(os.environ["PE_WORKSPACE_DIR"], "Tools")))
print sys.path
import PyClient
import PyClient.assetmanagercommon


root = os.environ["PE_WORKSPACE_DIR"] # this is a path to the workspace directory. Pyengine2 is in that directory
pyengine2Root = os.path.join(root, 'PrimeEngine')

def main():
    if len(sys.argv) < 2:
        print 'PE: TOOLS: ERROR: no package name provided as argument'
        return # no argument for package name
    package = sys.argv[1]
    print 'Have request to deploy package', package
    #use common API to retrieve list of packages
    availPackages = PyClient.assetmanagercommon.getListOfAvailablePackages()
    if not package in availPackages:
        print 'PE: TOOLS: ERROR: package %s was not found in current PyengineWorkspace' % (package,)
        return
    availTypesInPackage = PyClient.assetmanagercommon.getListOfAvailableAssetTypesForPacakge(package)
    conf = {}
    execfile(os.path.join(os.environ["PE_WORKSPACE_DIR"], 'GlobalConfig', 'Dirs.py'), conf)
    
    platform = "IOS" #default target platform
    
    if len(sys.argv) > 2:
        platform = sys.argv[2]

    print 'Deployment platform:', platform
    if platform == 'IOS':
        if (len(sys.argv) < 4):
            print 'PE: TOOLS: ERROR: not target path provided fro IOS option. Dont know where to deploy files.'
            return 
        targetWorkspace = sys.argv[3]
    else:
        targetWorkspace = "."
    
    print "Available asset types in the %s package" % (package,)
    print availTypesInPackage
    for t in availTypesInPackage:
        targetAssetPath = os.path.join(targetWorkspace ,'AssetsOut', package, t)
        files = PyClient.assetmanagercommon.getListOfAvailableAssetsForPacakgeOfType(package, t)
        for f in files:
            filePath = PyClient.assetmanagercommon.getAssetFullPath(package, t, f)
            fullTargetPath = os.path.join(targetAssetPath, f)
            print "Try to copy file:", filePath
            print 'to'
            print fullTargetPath
            if platform == 'IOS':
                os.system('mkdir -p "%s"' % os.path.split(fullTargetPath)[0])
                os.system('cp -f "%s" "%s"' % (filePath, fullTargetPath))
            else:
                print "Unsupported platform"
            
if __name__ == "__main__":
    main()
