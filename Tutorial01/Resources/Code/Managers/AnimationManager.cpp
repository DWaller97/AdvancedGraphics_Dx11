#include "AnimationManager.h"

AnimationManager* AnimationManager::m_reference = nullptr;


AnimationManager* AnimationManager::GetReference()
{
    if (m_reference)
        return m_reference;
    m_reference = new AnimationManager();
    return m_reference;
}

void AnimationManager::AddModels(Animation::Model* _models, int _modelCount = 1)
{
    for (int i = 0; i < _modelCount; i++)
        m_models.push_back(_models[i]);
}

void AnimationManager::Initialise(ID3D11DeviceContext* _context)
{
    m_deviceContext = _context;
}

/*********************************************************************************************************
* MARKING SCHEME 2021
* CATEGORY: WALKING ANIMATION
* DESCRIPTION: The animation is "animated" here. Every update, each animation stored within the
* list of models provided is iterated over, with the specific frame of animation calculated based on
* the delta time and the current frame. It then iterates through each joint/bone within this model's
* animation and changes the orientation and position of the joint/bone based on the position in the
* hierarchy, the previous joint/bone's position and orientation, as well as the current frame's position
* and orientation. It then moves the vertices of the mesh based on the bone/joint orientation and
* the weighting. Afterwards it stores this information in the subset's vertex buffer to be drawn later.
* Normals were not implemented due to time constraints, especially considering my model and 
* animation's limited visual appeal.
* [Part 2]
* *******************************************************************************************************/

void AnimationManager::Update(float _deltaTime)
{
    for (int i = 0; i < m_models.size(); i++) {

        Animation::Model* currModel = &m_models[i];
        int currAnimation = currModel->currAnimation;

        if (currAnimation >= currModel->animations.size())
            continue;

        Animation::Anim& currAnim = currModel->animations[currAnimation];


        currAnim.currAnimTime += _deltaTime;
        if (currAnim.currAnimTime >= currAnim.totalAnimTime)
            currAnim.currAnimTime = 0.0f;

        float currFrame = currAnim.currAnimTime * currAnim.framerate;
        int frame0 = floorf(currFrame);
        int frame1 = frame0 + 1;

        if (frame0 == currAnim.frameCount - 1)
            frame1 = 0;

        float diff = currFrame - frame0;
        vector<Animation::Joint> skeleton;

        for (int j = 0; j < currAnim.jointCount; j++) {
            Animation::Joint joint = {};
            Animation::Joint joint0 = currAnim.frameSkeleton[frame0][j];
            Animation::Joint joint1 = currAnim.frameSkeleton[frame1][j];

            joint.parentID = joint0.parentID;

            XMVECTOR joint0Orientation = XMVectorSet(joint0.orientation.x, joint0.orientation.y, joint0.orientation.z, joint0.orientation.w);
            XMVECTOR joint1Orientation = XMVectorSet(joint1.orientation.x, joint1.orientation.y, joint1.orientation.z, joint1.orientation.w);

            joint.position.x = joint0.position.x + (diff * (joint1.position.x - joint0.position.x));
            joint.position.y = joint0.position.y + (diff * (joint1.position.y - joint0.position.y));
            joint.position.z = joint0.position.z + (diff * (joint1.position.z - joint0.position.z));

            XMStoreFloat4(&joint.orientation, XMQuaternionSlerp(joint0Orientation, joint1Orientation, diff));
            skeleton.push_back(joint);

        }

        for (int j = 0; j < currModel->subsetCount; j++) {
            Animation::ModelSubset& currSubset = currModel->subset[j];
            for (int k = 0; k < currSubset.vertices.size(); k++) {
                BasicVertex v = currSubset.vertices[k];
                v.pos = XMFLOAT3(0, 0, 0);
                v.normal = XMFLOAT3(0, 0, 0);
                for (int l = 0; l < v.weightCount; l++) {
                    Animation::Weight weight = currSubset.weights[v.startWeight + l];
                    Animation::Joint joint = skeleton[weight.jointID];

                    XMVECTOR jointOrientation = XMVectorSet(joint.orientation.x, joint.orientation.y, joint.orientation.z, joint.orientation.w);
                    XMVECTOR weightPos = XMVectorSet(weight.position.x, weight.position.y, weight.position.z, 0.0f);

                    XMVECTOR jointOrientationConjugate = XMQuaternionInverse(jointOrientation);
                    XMFLOAT3 rotatedPoint;
                    XMStoreFloat3(&rotatedPoint, XMQuaternionMultiply(XMQuaternionMultiply(jointOrientation, weightPos), jointOrientationConjugate));

                    v.pos.x += (joint.position.x + rotatedPoint.x) * weight.bias;
                    v.pos.y += (joint.position.y + rotatedPoint.y) * weight.bias;
                    v.pos.z += (joint.position.z + rotatedPoint.z) * weight.bias;

                    //TODO: Normals calculations here

                }
                currSubset.positions[k] = v.pos;
                //Store normals here

            }
            for (int k = 0; k < currSubset.vertices.size(); k++) {
                currSubset.vertices[k].pos = currSubset.positions[k];
            }
            D3D11_MAPPED_SUBRESOURCE res;
            m_deviceContext->Map(currSubset.vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
            memcpy(res.pData, &currSubset.vertices[0], sizeof(BasicVertex) * currSubset.vertices.size());
            m_deviceContext->Unmap(currSubset.vertexBuffer, 0);
        }
    }
}
