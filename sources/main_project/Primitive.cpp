#include "Primitive.hpp"
#include "Primitive.hpp"

Primitive::Primitive(SceneObject* owner) : owner(owner)
{

}

Primitive::~Primitive()
{

}

SceneObject* Primitive::GetOwnerSceneObject() const
{
	return owner;
}
