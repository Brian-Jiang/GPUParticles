def TriangulateSplitAllMeshes(pScene, pManager):
    lNode = pScene.GetRootNode()
    lConverter = FbxGeometryConverter(pManager)
    
    if lNode:
        for i in range(lNode.GetChildCount()):
            lChildNode = lNode.GetChild(i)
            if lChildNode.GetNodeAttribute() != None:
                lAttributeType = (lChildNode.GetNodeAttribute().GetAttributeType())
            
                if lAttributeType == FbxNodeAttribute.eMesh:
                    lMesh = lChildNode.GetNodeAttribute()
                
                    print("\nMESH NAME :: %s" % lMesh.GetName())
                    print("MESH POLYGONS :: %i" % lMesh.GetPolygonCount())
                    print("MESH EDGES :: %i" % lMesh.GetMeshEdgeCount())     
                    print("TRIANGULATING MESH")
                    lTriangulatedMesh = lConverter.Triangulate(lMesh, False)
                    print("\nTRIANGULATING MESH COMPLETED")
                    print("TRIANGULATED MESH POLYGONS :: %i" % lTriangulatedMesh.GetPolygonCount())
                    print("TRIANGULATED MESH EDGES :: %i" % lTriangulatedMesh.GetMeshEdgeCount())                
                
                    lChildNode.RemoveNodeAttribute(lMesh)
                    lChildNode.AddNodeAttribute(lTriangulatedMesh)
                
                    # Mesh is triangulated, we can now split it per material
                    lResult = lConverter.SplitMeshPerMaterial(lTriangulatedMesh, False) 
                    #lChildNode.RemoveNodeAttribute(lTriangulatedMesh)       
                
def ListAllMeshesCount(pScene):
    print("NUMBER OF GEOMETRIES :: %i" % pScene.GetGeometryCount())
                
if __name__ == "__main__":
    try:
        from FbxCommon import *
        import sip
    except ImportError:
        import platform
        msg = 'You need to copy the content in compatible subfolder under /lib/python<version> into your python install folder such as '
        if platform.system() == 'Windows' or platform.system() == 'Microsoft':
            msg += '"Python26/Lib/site-packages"'
        elif platform.system() == 'Linux':
            msg += '"/usr/local/lib/python2.6/site-packages"'
        elif platform.system() == 'Darwin':
            msg += '"/Library/Frameworks/Python.framework/Versions/2.6/lib/python2.6/site-packages"'        
        msg += ' folder.'
        print(msg) 
        sys.exit(1)

    # Prepare the FBX SDK.
    lSdkManager, lScene = InitializeSdkObjects()

    # The example can take a FBX file as an argument.
    if len(sys.argv) > 1:
        print("\n\nFile: %s\n" % sys.argv[1])
        lResult = LoadScene(lSdkManager, lScene, sys.argv[1])
    else :
        lResult = False

        print("\n\nUsage: SplitMeshPerMaterial <FBX file name>\n")

    if not lResult:
        print("\n\nAn error occurred while loading the scene...")
    else :
        print("BEFORE SPLITTING MESHES")
        ListAllMeshesCount(lScene)
        TriangulateSplitAllMeshes(lScene, lSdkManager)
        
        print("\nAFTER SPLITTING MESHES")            
        ListAllMeshesCount(lScene)
        
        SaveScene(lSdkManager, lScene, "multiplematerials_output.fbx")
        
    # Destroy all objects created by the FBX SDK.
    lSdkManager.Destroy()
   
    sys.exit(0)
