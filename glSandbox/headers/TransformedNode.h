#pragma once
#include "Node.h"
#include "Util.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <type_traits>

class Translation
{
protected:
	glm::vec3 localTranslation{0.0f};

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

};

class Scale
{
protected:
	glm::vec3 localScale{1.0f};

public:
	void setLocalScale(glm::vec3 localScale)
	{
		this->localScale = localScale;
	}

	glm::mat4 getLocalScalingMatrix() const
	{
		return glm::scale(glm::mat4{1.0f}, localScale);
	}

};

//class Rotation
//{
//public:
//	virtual void setLocalRotation(glm::vec3 localRotation) = 0;
//	virtual glm::mat4 getLocalRotationMatrix() const = 0;
//
//};

class Rotation
{
protected:
	glm::quat localRotation;

public:
	void setLocalRotation(glm::quat localRotation)
	{
		this->localRotation = localRotation;
	}

	glm::mat4 getLocalRotationMatrix() const
	{
		return glm::mat4_cast(localRotation);
	}

};

//class RotationEulerXYZ : public Rotation
//{
//protected:
//	glm::vec3 localRotation{0.0f};
//
//public:
//	void setLocalRotation(glm::vec3 localRotation) override
//	{
//		this->localRotation = localRotation;
//	}
//
//	glm::mat4 getLocalRotationMatrix() const override
//	{ 
//		return glm::eulerAngleXYZ(localRotation.x, localRotation.y, localRotation.z);
//	}
//
//};
//
//class RotationEulerYX : public Rotation
//{
//protected:
//	glm::vec2 localRotation{0.0f};
//
//public:
//	void setLocalRotation(glm::vec3 localRotation) override
//	{
//		this->localRotation = localRotation;
//	}
//
//	glm::mat4 getLocalRotationMatrix() const override
//	{
//		return glm::eulerAngleYX(localRotation.y, localRotation.x);
//	}
//
//};

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
	template<typename Base> 
	static constexpr bool isTransformUsed = std::is_base_of_v<Base, Transformed<TransformTypes...>>;

public:
	void setLocalTransformation(glm::mat4&& m) override
	{
		auto[t, r, s] = decomposeTransformation(m);

		if constexpr(isTransformUsed<Translation>)
			static_cast<Translation*>(this)->setLocalTranslation(t);
		if constexpr(isTransformUsed<Rotation>)
			static_cast<Rotation*>(this)->setLocalRotation(r);
		if constexpr(isTransformUsed<Scale>)
			static_cast<Scale*>(this)->setLocalScale(s);
	}

	glm::mat4 getLocalTransformation() const override
	{
		glm::mat4 transformation{1.0f};

		if constexpr(isTransformUsed<Translation>)
			transformation *= static_cast<Translation const*>(this)->getLocalTranslationMatrix();
		if constexpr(isTransformUsed<Rotation>)
			transformation *= static_cast<Rotation const*>(this)->getLocalRotationMatrix();
		if constexpr(isTransformUsed<Scale>)
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

};

using TransformedNode = Transformed<Translation, Rotation, Scale>;