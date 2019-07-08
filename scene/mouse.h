#pragma once
#include <memory>
#include "dxm.h"
#include "camera.h"


namespace agv {

	class IMouseObserver
	{
	public:
		virtual void MouseMove(int x, int y) = 0;
		virtual void MouseLeftDown() = 0;
		virtual void MouseLeftUp() = 0;
		virtual void MouseMiddleDown() = 0;
		virtual void MouseMiddleUp() = 0;
		virtual void MouseRightDown() = 0;
		virtual void MouseRightUp() = 0;
		virtual void MouseWheel(int d) = 0;
	};


	namespace scene {
		class Node;
		class OrbitMover : public IMouseObserver
		{
			std::shared_ptr<CameraBase> m_camera;

			float m_yaw = 0;
			float m_pitch = 0;
			float m_distance = 3.0f;
			float m_shiftX = 0;
			float m_shiftY = 0;

			bool m_mouseLeftIsDown = false;
			bool m_mouseMiddleIsDown = false;
			bool m_mouseRightIsDown = false;
			int m_mouseLastX = 0;
			int m_mouseLastY = 0;

		public:
			OrbitMover(const std::shared_ptr<CameraBase> &camera)
				: m_camera(camera)
			{
				Update();
			}

			void MouseMove(int x, int y);
			void MouseLeftDown();
			void MouseLeftUp();
			void MouseMiddleDown();
			void MouseMiddleUp();
			void MouseRightDown();
			void MouseRightUp();
			void MouseWheel(int d);

		private:
			void Update();
		};
	}
}