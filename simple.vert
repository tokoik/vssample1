#version 330

// ローカル座標系の頂点位置
layout (location = 0) in vec4 pv;

// ローカル座標系からクリッピング座標系への変換行列
uniform mat4 mc;

// 時刻
uniform float t;

// 点の大きさ
const float ps = 10.0;

// 上端と下端の位置
const vec4 top = vec4(0.0, 1.0, 0.0, 0.0);
const vec4 bottom = vec4(0.0, -1.0, 0.0, 0.0);

void main()
{
  vec4 p = vec4(pv.x, fract(pv.y * 0.5 + 0.5 - t) * 2.0 - 1.0, pv.zw);
  gl_Position = mc * p;
  gl_PointSize = ps / gl_Position.w;
}
