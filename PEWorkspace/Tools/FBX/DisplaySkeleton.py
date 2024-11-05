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

from DisplayCommon import *
from fbx import FbxSkeleton, FbxNode, FbxMatrix, FbxVector4, FbxQuaternion
import sys
def DisplaySkeleton(pNode, peParserCtx):
    lSkeleton = pNode.GetNodeAttribute()

    if peParserCtx.logLevel > 0:
        DisplayString("Skeleton/Joint Name: ", pNode.GetName())
    peParserCtx.directMap[pNode.GetName()]['isJoint'] = True
    lSkeletonTypes = [ "Root", "Limb", "Limb Node", "Effector" ]

    peParserCtx.directMap[pNode.GetName()]['jointType'] = lSkeletonTypes[lSkeleton.GetSkeletonType()]
    
    if peParserCtx.logLevel > 0:
        
        DisplayString("    Type: ", peParserCtx.directMap[pNode.GetName()]['jointType'])
        
        if lSkeleton.GetSkeletonType() == FbxSkeleton.eLimb:
            DisplayDouble("    Limb Length: ", lSkeleton.LimbLength.Get())
        elif lSkeleton.GetSkeletonType() == FbxSkeleton.eLimbNode:
            DisplayDouble("    Limb Node Size: ", lSkeleton.Size.Get())
        elif lSkeleton.GetSkeletonType() == FbxSkeleton.eRoot:
            DisplayDouble("    Limb Root Size: ", lSkeleton.Size.Get())

    #print dir(FbxNode)
    #preRot = pNode.GetPreRotation(FbxNode.eSourcePivot)
    #peParserCtx.directMap[pNode.GetName()]['preRotation'] = preRot
    #
    #preRotMatrix = FbxMatrix()
    #preRotMatrix.SetTRS(FbxVector4(0, 0, 0), preRot, FbxVector4(1, 1, 1))
    #peParserCtx.directMap[pNode.GetName()]['preRotationMatrix'] = preRotMatrix
    #
    #postRot = pNode.GetPostRotation(FbxNode.eSourcePivot)
    #peParserCtx.directMap[pNode.GetName()]['postRotation'] = postRot
    #
    #
    #postRotMatrix = FbxMatrix()
    #postRotMatrix.SetTRS(FbxVector4(0, 0, 0), postRot, FbxVector4(1, 1, 1))
    #peParserCtx.directMap[pNode.GetName()]['postRotationMatrix'] = postRotMatrix
    #
    #peParserCtx.directMap[pNode.GetName()]['postRotationMatrixInverse'] = postRotMatrix.Inverse() #for some reason, fbx stores maya node's "Rotate Axis" as Inverse of Matrix("Rotate Axis")
    
    #DisplayColor("    Color: ", lSkeleton.GetLimbNodeColor())
    #print dir(pNode)
