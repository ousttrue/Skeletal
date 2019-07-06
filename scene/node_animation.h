#pragma once
#include "animation_time.h"
#include "dxm.h"


namespace agv {
	namespace scene {

		typedef dxm::Tree<class NodeValue> Node;
		class INodeAnimation
		{
		public:
			virtual void Update(Node *pNode, const AnimationTime &time) = 0;
		};


		class NodeRotation : public INodeAnimation
		{
			float m_angularVelocityRadians;
			float m_angle = 0;

		public:
			NodeRotation(float angularVelocity)
				: m_angularVelocityRadians(DirectX::XMConvertToRadians(angularVelocity))
			{}

			void Update(Node *pNode, const AnimationTime &time)override;
		};
	}
}
