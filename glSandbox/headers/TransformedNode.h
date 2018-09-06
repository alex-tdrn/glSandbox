#pragma once
#include "Node.h"
#include "Util.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <type_traits>

class Translation
{
private:
	glm::vec3 localTranslation{0.0f};

public:
	Translation() = default;
	virtual ~Translation() = default;

public:
	glm::vec3& getLocalTranslation()
	{
		return localTranslation;
	}

	void setLocalTranslation(glm::vec3 localTranslation)
	{
		this->localTranslation = localTranslation;
	}

	glm::mat4 getLocalTranslationMatrix() const
	{
		return glm::translate(glm::mat4{1.0f}, localTranslation);
	}

	void translate(glm::vec3 amount)
	{
		localTranslation += amount;
	}

};



class Scale
{
private:
	glm::vec3 localScale{1.0f};

public:
	Scale() = default;
	virtual ~Scale() = default;

public:
	void setLocalScale(glm::vec3 localScale)
	{
		this->localScale = localScale;
	}

	void setLocalScale(float localScale)
	{
		this->localScale = glm::vec3{localScale};
	}

	glm::vec3 getLocalScale() const
	{
		return localScale;
	}

	glm::mat4 getLocalScalingMatrix() const
	{
		return glm::scale(glm::mat4{1.0f}, localScale);
	}

};

class Rotation
{
private:
	glm::quat localRotation{};
	bool limitPitch = false;
	float localPitch = 0.0f;

public:
	Rotation() = default;
	virtual ~Rotation() = default;

public:
	void setLocalRotation(glm::quat localRotation)
	{
		localPitch = glm::eulerAngles(localRotation).x;
		this->localRotation = localRotation;
	}

	void setLocalRotation(glm::vec3 localRotation)
	{
		localPitch = localRotation.x;
		this->localRotation = glm::quat(localRotation);
	}

	glm::quat getLocalRotation() const
	{
		return localRotation;
	}

	void setLimitPitch(bool b)
	{
		limitPitch = b;
	}

	bool getLimitPitch() const
	{
		return limitPitch;
	}

	glm::mat4 getLocalRotationMatrix() const
	{
		return glm::mat4_cast(localRotation);
	}

	void rotate(float yawAmount, float pitchAmount)
	{
		if(limitPitch)
		{
			float const resultingPitch = localPitch + pitchAmount;
			float const upperLimit = 90.0f;
			float const lowerLimit = -90.0f;
			if(resultingPitch > upperLimit)
				pitchAmount = upperLimit - resultingPitch;
			else if(resultingPitch < lowerLimit)
				pitchAmount = lowerLimit - resultingPitch;
		}
		localPitch += pitchAmount;
		localRotation = glm::angleAxis(glm::radians(yawAmount), glm::vec3{0.0f, 1.0f, 0.0f})
			* localRotation * glm::angleAxis(glm::radians(pitchAmount), glm::vec3{1.0f, 0.0f, 0.0f});
	}

};

template <typename... TransformTypes>
class Transformed : public Node, public TransformTypes...
{
public:
	Transformed() = default;
	Transformed(Transformed const&) = delete;
	Transformed(Transformed&&) = default;
	Transformed(std::vector<std::unique_ptr<Node>>&& children)
		:Node(std::move(children))
	{
	}
	virtual ~Transformed() = default;
	Transformed& operator=(Transformed const&) = delete;
	Transformed& operator=(Transformed&&) = delete;

private:
	/*template<typename Base>
	static constexpr bool isTransformUsed()
	{
		return is_one_of<Base, TransformTypes...>::value;
	}*/
	//https://developercommunity.visualstudio.com/content/problem/317439/158-c-ice-invalid-if-constexpr.html
	/*template<typename Base> 
	static constepxr bool isTransformUsed = is_one_of<Base, TransformTypes...>::value;*/
	//"non constant sub-expression
	template<typename Base>
	using isTransformUsed = is_one_of<Base, TransformTypes...>;

public:
	void setLocalTransformation(glm::mat4&& m) override
	{
		auto[t, r, s] = decomposeTransformation(m);
		if constexpr(isTransformUsed<Translation>::value)
		{
			static_cast<Translation*>(this)->setLocalTranslation(t);
		}
		if constexpr(isTransformUsed<Rotation>::value)
		{
			static_cast<Rotation*>(this)->setLocalRotation(r);
		}
		if constexpr(isTransformUsed<Scale>::value)
		{
			static_cast<Scale*>(this)->setLocalScale(s);
		}
	}

	glm::mat4 getLocalTransformation() const override
	{
		glm::mat4 transformation{1.0f};

		if constexpr(isTransformUsed<Translation>::value)
			transformation *= static_cast<Translation const*>(this)->getLocalTranslationMatrix();
		if constexpr(isTransformUsed<Rotation>::value)
			transformation *= static_cast<Rotation const*>(this)->getLocalRotationMatrix();
		if constexpr(isTransformUsed<Scale>::value)
			transformation *= static_cast<Scale const*>(this)->getLocalScalingMatrix();

		return transformation;
	}

	glm::mat4 getGlobalTransformation() const override
	{
		if(parent == nullptr)
			return getLocalTransformation();
		else
			return parent->getGlobalTransformation() * getLocalTransformation();
	}

	template<typename Dummy = void>
	std::enable_if_t<isTransformUsed<Translation>::value && isTransformUsed<Rotation>::value, Dummy>
		rotateAroundPointInFront(float const distance, float const yawAmount, float const pitchAmount)
	{
		glm::vec3 pivot = glm::vec4{0.0f, 0.0f, distance, 1.0f} *glm::inverse(static_cast<Rotation*>(this)->getLocalRotationMatrix());
		static_cast<Rotation*>(this)->rotate(yawAmount, pitchAmount);
		glm::vec3 rotatedPivot = glm::vec4{0.0f, 0.0f, distance, 1.0f} *glm::inverse(static_cast<Rotation*>(this)->getLocalRotationMatrix());
		glm::vec3 diff = rotatedPivot - pivot;
		static_cast<Translation*>(this)->translate(diff);
	}

	void drawUI()
	{
		Node::drawUI();
		IDGuard idGuard{this};
		auto[globalTranslation, globalRotation, globalScale] = decomposeTransformation(getGlobalTransformation());
		if constexpr(isTransformUsed<Translation>::value)
		{
			ImGui::Text("Global Translation");
			ImGui::Text("%.2f, %.2f, %.2f", globalTranslation[0], globalTranslation[1], globalTranslation[2]);

			ImGui::Text("Local Translation");
			glm::vec3 localTranslation = static_cast<Translation*>(this)->getLocalTranslation();
			if(ImGui::DragFloat3("###Local Translation", &localTranslation[0], 0.01f))
				static_cast<Translation*>(this)->setLocalTranslation(localTranslation);

			if constexpr(isTransformUsed<Rotation>::value)
			{
				ImGui::Text("Translate Oriented");
				glm::vec3 translateOrientedAmount{0.0f};
				if(ImGui::DragFloat3("###Translate Oriented", &translateOrientedAmount[0], 0.01f))
					static_cast<Translation*>(this)->translate(static_cast<Rotation*>(this)->getLocalRotationMatrix() * glm::vec4{translateOrientedAmount, 1.0f});
			}
			
			ImGui::NewLine();
		}
		if constexpr(isTransformUsed<Rotation>::value)
		{
			ImGui::Text("Global Rotation (Quaternion)");
			ImGui::Text("%.2f, %.2f, %.2f, %.2f", globalRotation[0], globalRotation[1], globalRotation[2], globalRotation[3]);

			ImGui::Text("Global Rotation (Euler)");
			glm::vec3 globalRotationEuler = glm::degrees(glm::eulerAngles(globalRotation));
			ImGui::Text("%.2f, %.2f, %.2f", globalRotationEuler[0], globalRotationEuler[1], globalRotationEuler[2]);

			ImGui::Text("Local Rotation (Quaternion)");
			glm::quat localRotation = static_cast<Rotation*>(this)->getLocalRotation();
			ImGui::Text("%.2f, %.2f, %.2f, %.2f", localRotation[0], localRotation[1], localRotation[2], localRotation[3]);

			ImGui::Text("Local Rotation (Euler)");
			glm::vec3 localRotationEuler = glm::degrees(glm::eulerAngles(localRotation));
			ImGui::Text("%.2f, %.2f, %.2f", localRotationEuler[0], localRotationEuler[1], localRotationEuler[2]);

			static bool rotationStyleOrbital = false;
			static float rotationDistance = 1.0f;
			ImGui::Text("Rotate");
			if constexpr(isTransformUsed<Translation>::value)
			{
				ImGui::SameLine();
				ImGui::RadioButton("FPS", reinterpret_cast<int*>(&rotationStyleOrbital), 0);
				ImGui::SameLine();
				ImGui::RadioButton("Orbital", reinterpret_cast<int*>(&rotationStyleOrbital), 1);
			}
			bool limitPitch = static_cast<Rotation*>(this)->getLimitPitch();
			ImGui::SameLine();
			if(ImGui::Checkbox("Limit Pitch", &limitPitch))
				static_cast<Rotation*>(this)->setLimitPitch(limitPitch);
			if constexpr(isTransformUsed<Translation>::value)
			{
				if(rotationStyleOrbital)
					ImGui::DragFloat("Rotation Distance", &rotationDistance, 0.01f);
			}
			glm::vec3 rotateAmount{0.0f};
			if(ImGui::DragFloat3("###Rotate", &rotateAmount[0], 0.1f))
			{
				if constexpr(isTransformUsed<Translation>::value)
				{
					if(rotationStyleOrbital)
						this->rotateAroundPointInFront(rotationDistance, rotateAmount.y, rotateAmount.x);
					else
						static_cast<Rotation*>(this)->rotate(rotateAmount.y, rotateAmount.x);
				}
				else
				{
					static_cast<Rotation*>(this)->rotate(rotateAmount.y, rotateAmount.x);
				}
			}
			
			ImGui::NewLine();
		}
		if constexpr(isTransformUsed<Scale>::value)
		{
			ImGui::Text("Global Scale");
			ImGui::Text("%.2f, %.2f, %.2f", globalScale[0], globalScale[1], globalScale[2]);

			ImGui::Text("Local Scale");
			glm::vec3 localScale = static_cast<Scale*>(this)->getLocalScale();
			if(ImGui::DragFloat3("###Local Scale", &localScale[0], 0.01f))
			{
				if(ImGui::GetIO().KeyShift)
				{
					glm::vec3 currentLocalScale = static_cast<Scale*>(this)->getLocalScale();
					float uniformScale = 1.0f;
					for(int i = 0; i < 3; i++)
						if(localScale[i] != currentLocalScale[i])
							static_cast<Scale*>(this)->setLocalScale(localScale[i]);
				}
				else
					static_cast<Scale*>(this)->setLocalScale(localScale);
			}
			
			ImGui::NewLine();
		}
	}

};

using TransformedNode = Transformed<Translation, Rotation, Scale>;