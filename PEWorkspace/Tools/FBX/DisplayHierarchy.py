"""

 Copyright (C) 2001 - 2010 Autodesk, Inc. and/or its licensors.
 All Rights Reserved.

 The coded instructions, statements, computer programs, and/or related material 
 (collectively the "Data") in these files contain unpublished information 
 proprietary to Autodesk, Inc. and/or its licensors, which is protected by 
 Canada and United States of America federal copyright law and by international 
 treaties. 
 
 The Data may not be disclosed or distributed to third parties, in whole or in
 part, without the prior written consent of Autodesk, Inc. ("Autodesk").

 THE DATA IS PROVIDED "AS IS" AND WITHOUT WARRANTY.
 ALL WARRANTIES ARE EXPRESSLY EXCLUDED AND DISCLAIMED. AUTODESK MAKES NO
 WARRANTY OF ANY KIND WITH RESPECT TO THE DATA, EXPRESS, IMPLIED OR ARISING
 BY CUSTOM OR TRADE USAGE, AND DISCLAIMS ANY IMPLIED WARRANTIES OF TITLE, 
 NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE OR USE. 
 WITHOUT LIMITING THE FOREGOING, AUTODESK DOES NOT WARRANT THAT THE OPERATION
 OF THE DATA WILL BE UNINTERRUPTED OR ERROR FREE. 
 
 IN NO EVENT SHALL AUTODESK, ITS AFFILIATES, PARENT COMPANIES, LICENSORS
 OR SUPPLIERS ("AUTODESK GROUP") BE LIABLE FOR ANY LOSSES, DAMAGES OR EXPENSES
 OF ANY KIND (INCLUDING WITHOUT LIMITATION PUNITIVE OR MULTIPLE DAMAGES OR OTHER
 SPECIAL, DIRECT, INDIRECT, EXEMPLARY, INCIDENTAL, LOSS OF PROFITS, REVENUE
 OR DATA, COST OF COVER OR CONSEQUENTIAL LOSSES OR DAMAGES OF ANY KIND),
 HOWEVER CAUSED, AND REGARDLESS OF THE THEORY OF LIABILITY, WHETHER DERIVED
 FROM CONTRACT, TORT (INCLUDING, BUT NOT LIMITED TO, NEGLIGENCE), OR OTHERWISE,
 ARISING OUT OF OR RELATING TO THE DATA OR ITS USE OR ANY OTHER PERFORMANCE,
 WHETHER OR NOT AUTODESK HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH LOSS
 OR DAMAGE. 
 
"""
import sys
from fbx import FbxSkeleton, FbxNode, FbxMatrix, FbxVector4, FbxQuaternion
from Logger import Logger

def DisplayHierarchy(pScene, peParserCtx):
    peParserCtx.directMap = {}
    lRootNode = pScene.GetRootNode()
    
    peParserCtx.logger.StartScope("\n\n---------\nStep 1: Analyzing hierarchy (to know who is whose child)", True)
    
    peParserCtx.rootDict = DisplayNodeHierarchy(lRootNode, 0, None, "", peParserCtx)
    
    peParserCtx.logger.EndScope("  Found %d nodes in hierarchy\n---------" % len(peParserCtx.directMap.keys()), True)

def DisplayNodeHierarchy(pNode, pDepth, parent, parentName, peParserCtx):
    
    res = {'name':pNode.GetName(), 'fbxNode' : pNode, 'children':[], 'fullName':parentName+pNode.GetName(), 'parent':parent}
    
    lclTranslation = pNode.LclTranslation.Get()
    res['LclTranslation'] = lclTranslation
    lclRotation = pNode.LclRotation.Get()
    res['LclRotation'] = lclRotation
    scale = pNode.LclScaling.Get()
    res['LclScaling'] = scale
    
    preRot = pNode.GetPreRotation(FbxNode.eSourcePivot)
    res['preRotation'] = preRot
    
    preRotMatrix = FbxMatrix()
    preRotMatrix.SetTRS(FbxVector4(0, 0, 0), preRot, FbxVector4(1, 1, 1))
    res['preRotationMatrix'] = preRotMatrix
    
    postRot = pNode.GetPostRotation(FbxNode.eSourcePivot)
    res['postRotation'] = postRot
    
    postRotMatrix = FbxMatrix()
    postRotMatrix.SetTRS(FbxVector4(0, 0, 0), postRot, FbxVector4(1, 1, 1))
    res['postRotationMatrix'] = postRotMatrix
    
    res['postRotationMatrixInverse'] = postRotMatrix.Inverse() #for some reason, fbx stores maya node's "Rotate Axis" as Inverse of Matrix("Rotate Axis")
    
    if peParserCtx.directMap.has_key(pNode.GetName()):
        print("ERROR: a node uses a duplicate name. Rename it to something else in maya. Name: %s Full Name: %s Existing Entry: %s" % (pNode.GetName(), res['fullName'], peParserCtx.directMap[pNode.GetName()]['fullName']))
        sys.exit(0)
    peParserCtx.directMap[pNode.GetName()] = res
    if parent:
        parent['children'].append(res)
    lString = ""
    for i in range(pDepth):
        lString += "  "
    depthStr = lString[:]
    lString += pNode.GetName()
    lString += " full name: %s" % res['fullName']
    peParserCtx.logger.StartScope("[%s] T: %.2f %.2f %.2f R: %.2f %.2f %.2f S %.2f %.2f %.2f" %(res['name'], lclTranslation[0],lclTranslation[1],lclTranslation[2],
            lclRotation[0], lclRotation[1], lclRotation[2], scale[0], scale[1], scale[2]), collapse = False)
    
    peParserCtx.logger.StartScope("Detailed node contents")
    for k in ['name', 'fullName', 'LclTranslation', 'LclRotation', 'LclScaling', 'preRotation', 'postRotation', 'postRotationMatrix', 'postRotationMatrixInverse']:
        peParserCtx.logger.AddLine("'%s' : %s" % (k, Logger.FbxObjectToString(res[k])))
    peParserCtx.logger.EndScope()
    
    for i in range(pNode.GetChildCount()):
        DisplayNodeHierarchy(pNode.GetChild(i), pDepth + 1, res, res['fullName'] + '|', peParserCtx)
    
    peParserCtx.logger.EndScope()
    
    return res