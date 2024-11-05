
from DisplayCommon import *
from fbx import FbxSkeleton
def PrintJoint(joint, logger, offset = ''):
    logger.AddLine("%sJoint[%d] name [%s]" % (offset, joint['index'], joint['name']), True)
    for c in joint['children']:
        PrintJoint(c, logger, offset + '-')
        
def AnalyzeSkeletonHierarchy(hierarchy, directMap, logger, logLevel):
    logger.StartScope("------------\nStep 4: Analyse joint nodes + hierarchy => skeletons", True)

    logger.AddLine("AnalyzeSkeletonHierarchy Entry..", True)
    #print hierarchy
    skelRoots = []
    skelDirectMaps = []
    AnalyzeNode(hierarchy, skelRoots, skelDirectMaps, None, None, None, logLevel)
    summary = "  Found %d joint hierarchies:" % len(skelRoots)
    for root in skelRoots:
        summary += ('[root: "' + root['name'] + '" with ' + str(root['totalChildren'] + 1) + ' joints]')

    logger.AddLine(summary, True)
    
    for root in skelRoots:
        logger.AddLine("skelRoot %s:" % root['name'], True)
        PrintJoint(root, logger)
        logger.AddLine("\n", True)

    logger.EndScope("------------", True)
    return skelRoots, skelDirectMaps
    
def AnalyzeNode(node, skelRoots, skelDirectMaps, curRoot, curDirectMap, parentJoint, logLevel):
    if node.get('isJoint', False):
        joint = {'name':node['name'], 'children':[], 'totalChildren':0, 'preRotation' : node['preRotation'], 'preRotationMatrix' : node['preRotationMatrix'],
        'postRotation' : node['postRotation'], 'postRotationMatrix' : node['postRotationMatrix'], 'postRotationMatrixInverse' : node['postRotationMatrixInverse'],
        'LclTranslation' : node['LclTranslation'], 'LclRotation' : node['LclRotation'], 'LclScaling' : node['LclScaling'],
        'rotOrder' : node['rotOrder']}
        if parentJoint:
            joint['index'] = curRoot['numJointsInHierarchy']
            curRoot['numJointsInHierarchy'] += 1
            curDirectMap[joint['name']] = joint
            curDirectMap[joint['index']] = joint
            parentJoint['children'].append(joint)
        else:
            curRoot = joint
            joint['index'] = 0
            joint['numJointsInHierarchy'] = 1
            skelRoots.append(joint)
            curDirectMap = {joint['name']:joint}
            skelDirectMaps.append(curDirectMap)
            
        for childNode in node['children']:
            nextIndex = AnalyzeNode(childNode, skelRoots, skelDirectMaps, curRoot, curDirectMap, joint, logLevel)
                
        for jointChild in joint['children']:
            joint['totalChildren'] += 1 + jointChild['totalChildren']
        
    else:
        for childNode in node['children']:
            AnalyzeNode(childNode, skelRoots, skelDirectMaps, None, None, None, logLevel)
