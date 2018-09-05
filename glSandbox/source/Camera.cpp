#include "Camera.h"
#include "Globals.h"
#include "Util.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <imgui.h>

Camera::Camera()
{
	setLocalTranslation(glm::vec3{0.0f, 0.0f, 8.0f});
}

unsigned int Camera::ubo()
{
	static unsigned int ubo = [](){
		unsigned int ubo;
		glGenBuffers(1, &ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, 128, nullptr, GL_STREAM_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);
		return ubo;
	}();

	return ubo;
}

void Camera::use() const
{
	glBindBuffer(GL_UNIFORM_BUFFER, ubo());
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(getProjectionMatrix()));
	glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(getViewMatrix()));
}

std::string const& Camera::getName() const
{
	return name.get();
}

float Camera::getNearPlane() const
{
	return nearPlane;
}

float Camera::getFarPlane() const
{
	return farPlane;
}

glm::mat4 Camera::getProjectionMatrix() const
{
	if(projectionOrtho)
		return glm::ortho(-info::windowWidth * 0.5f * orthoScale, +info::windowWidth * 0.5f * orthoScale, -info::windowHeight* 0.5f * orthoScale, +info::windowHeight* 0.5f * orthoScale, nearPlane, farPlane);
	else
		return glm::perspective(glm::radians(fov), static_cast<float>(info::windowWidth) / info::windowHeight, nearPlane, farPlane);
}

glm::mat4 Camera::getViewMatrix() const
{
	return glm::inverse(getGlobalTransformation());
}

void Camera::move(glm::vec3 amount)
{
	amount[2] *= -1;
	//if(orbital)
		//localTranslation.z += amount.z;
	//else
		localTranslation += glm::mat3(getLocalRotationMatrix()) * amount;
}

void Camera::rotate(float yawAmount, float pitchAmount)
{
	if(orbital)
	{
		float const d = 2.0;
		glm::vec3 pivot = glm::vec4{0.0f, 0.0f, d, 1.0f} * glm::inverse(getLocalRotationMatrix());
		localRotation = glm::angleAxis(glm::radians(yawAmount), glm::vec3{0.0f, 1.0f, 0.0f}) * localRotation * glm::angleAxis(glm::radians(pitchAmount), glm::vec3{1.0f, 0.0f, 0.0f});
		glm::vec3 rotatedPivot = glm::vec4{0.0f, 0.0f, d, 1.0f} * glm::inverse(getLocalRotationMatrix());
		glm::vec3 diff = rotatedPivot - pivot;
		localTranslation += diff;
		/*glm::vec3 centerOfRotation = glm::vec4{localTranslation, 1.0f} - glm::vec4{0.0f, 0.0f, d, 1.0f} *glm::mat4_cast(localRotation);
		localTranslation -= centerOfRotation;
		float dist = glm::length(localTranslation);
		auto gggg = glm::angleAxis(glm::radians(0.0f), glm::vec3{0.0f, 1.0f, 0.0f}) * localRotation * glm::angleAxis(glm::radians(0.0f), glm::vec3{1.0f, 0.0f, 0.0f});
		localTranslation = glm::vec4{0.0f, 0.0f, glm::length(localTranslation), 1.0f} * glm::inverse(glm::mat4_cast(localRotation));

		localTranslation += centerOfRotation;*/

		//glm::vec3 rotatedPivot = glm::vec4{0.0f, 0.0f, pivotDistance, 1.0f} *getLocalRotationMatrix();
		//auto t = localTranslation - glm::vec3{pivot.x, pivot.y, pivot.z};

		//localTranslation -= rotatedPivot - pivot;
		//localTranslation = glm::vec3{pivot.x, pivot.y, pivot.z} + t;
		//auto diff = pivotDistance*(transformedPivot - pivot);
		//localTranslation += glm::vec3{diff.x, 0, -diff.z};
	}
	else
	{
		localRotation = glm::angleAxis(glm::radians(yawAmount), glm::vec3{0.0f, 1.0f, 0.0f}) * localRotation * glm::angleAxis(glm::radians(pitchAmount), glm::vec3{1.0f, 0.0f, 0.0f});

	}


	/*localRotation.y -= glm::radians(int(glm::degrees(localRotation.y)) / 360 * 360.0f);
	if(localRotation.y < 0)
		localRotation.y += glm::radians(360.0f);
	if(localRotation.x > glm::radians(89.0f))
		localRotation.x = glm::radians(89.0f);
	else if(localRotation.x < glm::radians(-89.0f))
		localRotation.x = glm::radians(-89.0f);*/
}

void Camera::drawUI()
{
	Node::drawUI();
	IDGuard idGuard{this};
	ImGui::BeginChild("Camera", {ImGui::GetTextLineHeightWithSpacing() * 22, ImGui::GetTextLineHeightWithSpacing() * 10});
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Style");
	ImGui::SameLine();
	ImGui::RadioButton("FPS", reinterpret_cast<int*>(&orbital), 0);
	ImGui::SameLine();
	ImGui::RadioButton("Orbital", reinterpret_cast<int*>(&orbital), 1);
	glm::vec3 move{0.0f};
	if(ImGui::DragFloat3("Move", &move[0], 0.01f))
		this->move(move);
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Projection");
	ImGui::SameLine();
	ImGui::RadioButton("Perspective", reinterpret_cast<int*>(&projectionOrtho), 0);
	ImGui::SameLine();
	ImGui::RadioButton("Orthographic", reinterpret_cast<int*>(&projectionOrtho), 1);
	if(projectionOrtho)
		ImGui::DragFloat("Scale", &orthoScale, 0.0001f);
	else
		ImGui::DragFloat("FOV", &fov, 0.1f);
	ImGui::DragFloat("Near Plane", &nearPlane, 0.01f);
	ImGui::DragFloat("Far Plane", &farPlane, 0.1f);
	ImGui::EndChild();
}