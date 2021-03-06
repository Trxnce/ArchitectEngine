#include "TransformComponent.h"
#include "RendererComponent.h"
#include "ShaderProgram.h"
#include "Transform.h"
#include "Core.h"
#include "Entity.h"

TransformComponent::TransformComponent()
{
	transform = std::make_shared<Transform>();
}

void TransformComponent::onTick()
{
	glm::mat4 mm = glm::mat4(1.0f);
	mm = transform->updateModelMatrix(mm, position, rotation, scale);
	transform->setModelMatrix(mm);
	
	if (getEntity()->getComponent<RendererComponent>()->getShader())
		getEntity()->getComponent<RendererComponent>()->getShader()->SetUniform("modelMatrix", mm);
	else
		getCore()->getShaderProgram()->SetUniform("modelMatrix", mm);
}
