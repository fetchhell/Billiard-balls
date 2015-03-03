#include <object/my_object.h>

//-------------------------------------
object::object() {

	NvBuildIdentityMatf(this->m_model);

	angleX = 0, angleY = 0;
	prevX = 0, prevY = 0;
}

//-------------------------------------
void object::scaleObject(vec3f scaleVector) {

	float scaleMatrix[4][4];
	NvBuildScaleMatf(scaleMatrix, scaleVector.x, scaleVector.y, scaleVector.z);

	float old_model[4][4];
	memcpy(old_model, this->m_model, sizeof(this->m_model));

	NvMultMatf(this->m_model, scaleMatrix, old_model);
}

//-------------------------------------
void object::translateObject(vec3f translateVector) {

	float translateMatrix[4][4];
	NvBuildTranslateMatf(translateMatrix, translateVector.x, translateVector.y, translateVector.z);

	float old_model[4][4];
	memcpy(old_model, this->m_model, sizeof(this->m_model));

	NvMultMatf(this->m_model, translateMatrix, old_model);
}

//-------------------------------------
void object::rotateX(float rotX) {

	float rotateXMatrix[4][4];
	NvBuildRotXDegMatf(rotateXMatrix, rotX);

	float old_model[4][4];
	memcpy(old_model, this->m_model, sizeof(this->m_model));

	NvMultMatf(this->m_model, rotateXMatrix, old_model);
}

//-------------------------------------
void object::rotateY(float rotY) {

	float rotateYMatrix[4][4];
	NvBuildRotYDegMatf(rotateYMatrix, rotY);

	float old_model[4][4];
	memcpy(old_model, this->m_model, sizeof(this->m_model));

	NvMultMatf(this->m_model, rotateYMatrix, old_model);
}

//-------------------------------------
void object::rotateZ(float rotZ) {

	float rotateZMatrix[4][4];
	NvBuildRotZDegMatf(rotateZMatrix, rotZ);

	float old_model[4][4];
	memcpy(old_model, this->m_model, sizeof(this->m_model));

	NvMultMatf(this->m_model, rotateZMatrix, old_model);
}

//-------------------------------------
void object::identity() {

	NvBuildIdentityMatf(this->m_model);
}