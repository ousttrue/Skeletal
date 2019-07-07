#pragma once

namespace agv::renderer
{
class GLES3Renderer;
}

void AddOnInit();
void AddOnDraw(agv::renderer::GLES3Renderer *renderer);
