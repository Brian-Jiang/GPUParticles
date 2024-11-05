import os
import sys
#files in PrimeEngine folder
root = os.environ["PE_WORKSPACE_DIR"] # this is a path to the workspace directory. PrimeEngine is in that directory

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
                    print "Unsupported platform"

def main():
    conf = {}
    execfile(os.path.join(os.environ["PE_WORKSPACE_DIR"], 'GlobalConfig', 'Dirs.py'), conf)
    
    platform = "IOS" #default target platform
    
    if len(sys.argv) > 2:
        platform = sys.argv[2]
    
    print 'Deployment platform:', platform
    if platform == 'IOS':
        if (len(sys.argv) < 4):
            print 'PE: TOOLS: ERROR: not target path provided for IOS option. Dont know where to deploy files.'
        
        targetWorkspace = sys.argv[3]
    else:
        targetWorkspace = "."
    
    #parse whole workspace instead of just Engine folder because other solutions might have lua files
    #todo: go through game soltuions only
    conf = {}
    execfile(os.path.join(os.environ["PE_WORKSPACE_DIR"], 'GlobalConfig', 'Dirs.py'), conf)
    processes = conf['LaunchOptions'].keys() + ["PrimeEngine"]
    for process in processes:
        cpyLuaAndLuahRecursive(os.path.join(root, "Code", process), os.path.join(targetWorkspace, "Code", process), platform)
if __name__ == "__main__":
    main()
    
