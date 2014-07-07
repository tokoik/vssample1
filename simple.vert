#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

// ���[�J�����W�n�̒��_�ʒu
layout (location = 0) in vec4 pv;

// ���[�J�����W�n����N���b�s���O���W�n�ւ̕ϊ��s��
uniform mat4 mc;

// ����
uniform float t;

// ��[�Ɖ��[�̈ʒu
const vec4 top = vec4(0.0, 1.0, 0.0, 0.0);
const vec4 bottom = vec4(0.0, -1.0, 0.0, 0.0);

void main()
{
  vec4 p = pv;//vec4(pv.x, fract(pv.y * 0.5 + 0.5 - t) * 2.0 - 1.0, pv.zw);
  gl_Position = mc * p;
  gl_PointSize = 3;//10.0 / gl_Position.w;
}
