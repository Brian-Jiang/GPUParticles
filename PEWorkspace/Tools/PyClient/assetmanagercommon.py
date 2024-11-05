import sys
import bootstrap
import os
import os.path
import exceptions
import zipfile
import shutil
import uuid

def getListOfAvailablePackages():
    conf = {}
    assetsTop = os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], 'AssetsOut')
    execfile(os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], 'GlobalConfig', 'Dirs.py'), conf)
    packages = filter(lambda x: os.path.isdir(os.path.join(assetsTop, x)) and x not in conf['AssetDirs'] and x not in conf['IgnoreDirs'], os.listdir(assetsTop))
    return packages
def getListOfAvailablePackageZips():
    conf = {}
    assetsTop = os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], 'AssetsOut')
    execfile(os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], 'GlobalConfig', 'Dirs.py'), conf)
    packages = filter(lambda x: x.endswith('.zip') and os.path.isfile(os.path.join(assetsTop, x)), os.listdir(assetsTop))
    return packages
def getAssetFullPath(packageName, assetType, asset):
    assetsTop = os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], 'AssetsOut')
    return os.path.join(assetsTop, packageName, assetType, asset)
def getListOfAvailableAssetTypesForPacakge(packageName):
    ''' retrurns the list of types of assets that are in the given package
        note this is essentially a list fo folders that are in package folder
        there is no guarantee that the folders (types) will not be empty
    '''
    assetsTop = os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], 'AssetsOut')
    packagePath = os.path.join(assetsTop, packageName)
    conf = {}
    execfile(os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], 'GlobalConfig', 'Dirs.py'), conf)
    if not os.path.isdir(packagePath):
        print "Error: the package path does not exist. Returnin empty list"
        return []
    contents = filter(lambda x: x in conf['AssetDirs'] and x not in conf['IgnoreDirs'], os.listdir(packagePath))
    return contents
    
def getListOfAvailableAssetsForPacakgeOfType(packageName, assetType):
    assetsTop = os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], 'AssetsOut')
    packagePath = os.path.join(assetsTop, packageName, assetType)
    conf = {}
    execfile(os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], 'GlobalConfig', 'Dirs.py'), conf)
    assets = []
    if os.path.isdir(packagePath):
        assets = filter(lambda x: x not in conf['AssetDirs'] and x not in conf['IgnoreDirs'] and not os.path.splitext(x)[1] in conf['IgnoreExt'], os.listdir(packagePath))
    else:
        print "Warning: Folder for asset type", assetType, "in package", packageName, "does not exist"
    return assets
def runMetaScript(packageName, scriptName):
    conf = {}
    assetsTop = os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], 'AssetsOut')
    execfile(os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], 'AssetsOut', packageName, 'GameObjectMetaScripts', scriptName), conf)
    return conf
def appendPathToAssetsIn(remainingPath):
    return os.path.normpath(os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], 'AssetsIn', remainingPath))
def deployPackageToXBox360(package):
    availPackages = getListOfAvailablePackages()
    if not package in availPackages:
        print 'PYENGINE: TOOLS: ERROR: package %s was not found in current PyengineWorkspace' % (package,)
        return
    availTypesInPackage = getListOfAvailableAssetTypesForPacakge(package)
    conf = {}
    execfile(os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], 'GlobalConfig', 'Dirs.py'), conf)
    targetWorkspace = conf['X360WorkspacePath']
    
    print "Available asset types in the %s package" % (package,)
    print availTypesInPackage
    for t in availTypesInPackage:
        targetAssetPath = targetWorkspace + '\\AssetsOut\\' + package + '\\' + t + '\\'
        files = getListOfAvailableAssetsForPacakgeOfType(package, t)
        for f in files:
            filePath = getAssetFullPath(package, t, f)
            fullTargetPath = targetAssetPath + f
            print "Try to copy file:", filePath
            print 'to'
            print fullTargetPath
            os.system('XbCp /d /Y /t "%s" "%s"' % (filePath, fullTargetPath))
def exportPackage(package):
    availPackages = getListOfAvailablePackages()
    if not package in availPackages:
        print 'PYENGINE: TOOLS: ERROR: package %s was not found in current PyengineWorkspace' % (package,)
        return
    file = zipfile.ZipFile(os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], "AssetsOut", package+'.zip'), 'w')
    availTypesInPackage = getListOfAvailableAssetTypesForPacakge(package)
    print "Available asset types in the %s package" % (package,)
    print availTypesInPackage
    for t in availTypesInPackage:
        targetAssetPath = package + '\\' + t + '\\'
        files = getListOfAvailableAssetsForPacakgeOfType(package, t)
        for f in files:
            filePath = getAssetFullPath(package, t, f)
            fullTargetPath = targetAssetPath + f
            print "Try to add file to zip:", filePath
            print 'as zipmember'
            print fullTargetPath
            file.write(filePath, fullTargetPath)
    file.printdir()
    file.close()
def importPackage(package):
    availPackages = getListOfAvailablePackageZips()
    if not package in availPackages:
        print 'PYENGINE: TOOLS: ERROR: package %s was not found in current PyengineWorkspace' % (package,)
        return
    file = zipfile.ZipFile(os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], "AssetsOut", package), 'r')
    file.extractall(os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], "AssetsOut"))
    file.close()
def getListOfAvailableProjects():
    workspace = os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"])
    candidates = filter(lambda x: os.path.isdir(os.path.join(workspace, x)) and not x.endswith('.svn'), os.listdir(workspace))
    print candidates    
    projects = []
    for c in candidates:
        for f in os.listdir(os.path.join(workspace, c)):
            if os.path.isfile(os.path.join(workspace, c, f)) and f.endswith('.vcproj') and f[:-len('.vcproj')].lower() == c.lower():
                projects.append(c)
                break
    print "PE: TOOLS: assetmanagercommon.py: getListOfAvailableProjects(): found projects:", projects
    return projects
def cloneProject(projName, newProj):
    conf = {}
    execfile(os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], 'GlobalConfig', 'Dirs.py'), conf)
    ignores = conf['ProjectCloneIgnores']
    
    projdir = os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], projName)
    destdir = os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], newProj)
    shutil.rmtree(destdir, True)
    shutil.copytree(projdir, destdir, False, lambda x,y : filter(lambda z: len(filter(lambda w: z.endswith(w), ignores)), y))
    newProjPath = os.path.join(destdir, newProj + '.vcproj')
    oldProjPath = os.path.join(destdir, projName+'.vcproj')
    
    
    #need to parse .vcproj file and rename references to old project and generate a new GUID
    fin = open(oldProjPath, 'r')
    fout = open(newProjPath, 'w')
    for l in fin.readlines():
        outline = l
        #check for Name="ProjName"
        if (l.find("Name=") != -1 or l.find("RootNamespace=") != -1):
            firstQuote = l.find('"')
            secondQuote = l.find('"', firstQuote+1)
            if l[firstQuote+1 : secondQuote].lower() == projName.lower():
                # only replace name="<projname>"
                outline = l[:firstQuote+1] + newProj +  l[secondQuote:]
        elif (l.find("ProjectGUID") != -1):
            firstQuote = l.find('"')
            secondQuote = l.find('"', firstQuote+1)
            outline = l[:firstQuote+1] + "{" + str(uuid.uuid1()) + "}" +  l[secondQuote:]
        fout.write(outline)
    fin.close()
    fout.close()
    os.remove(oldProjPath)
    
def cpyLuaAndLuahRecursive(srcDir, targetDir, platform):
    names = os.listdir(srcDir)
    for name in names:
        fullSrcPath = os.path.join(srcDir, name)
        if name != '.svn' and os.path.isdir(fullSrcPath):
            #recursively continue
            cpyLuaAndLuahRecursive(fullSrcPath, os.path.join(targetDir, name), platform)
        else:
            ext = os.path.splitext(name)[1]
            if ext == '.lua' or ext == '.luah':
                #copy the file
                fullTargetPath = os.path.join(targetDir, name)
                print "Copying: %s to %s" % (fullSrcPath, fullTargetPath)
                if platform == 'IOS':
                    os.system('mkdir -p "%s"' % os.path.split(fullTargetPath)[0])
                    os.system('cp -f "%s" "%s"' % (fullSrcPath, fullTargetPath))
                else:
                    os.system('XbCp /d /Y /t "%s" "%s"' % (fullSrcPath, fullTargetPath))
def deployEngine(platform):

    conf = {}
    execfile(os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], 'GlobalConfig', 'Dirs.py'), conf)
    targetWorkspace = conf['X360WorkspacePath'] 
    #parse whole workspace instead of just Engine folder because other solutions might have lua files
    #todo: go trhough game soltuions only
    processes = conf['LaunchOptions'].keys() + ["PrimeEngine"]
    for process in processes:
        cpyLuaAndLuahRecursive(os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], "Code", process), os.path.join(targetWorkspace, "Code", process), platform)
    