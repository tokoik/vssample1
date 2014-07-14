#version 330

// フレームバッファに出力するデータ
layout (location = 0) out vec4 fc;                  // フラグメントの色

void main()
{
  fc = vec4(vec3(1.0 - gl_FragCoord.z), 1.0);
}
