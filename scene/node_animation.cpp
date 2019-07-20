#include "node_animation.h"
#include "node.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <DirectXMath.h>

namespace skeletal::scene
{

void NodeRotation::Update(Node *pNode, const AnimationTime &time)
{
    m_angle += m_angularVelocityRadians * time.DeltaSeconds;
    auto r = DirectX::XMMatrixRotationZ(m_angle);
    pNode->SetLocalMatrix(r);
}

} // namespace skeletal::scene
